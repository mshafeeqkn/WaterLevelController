#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/i2c.h>

#define DEVICE_NAME     "wlc"
#define CLASS_NAME      "wlc_class"
#define STATIC_MAJOR    64
#define MESSAGE_LEN     24
#define STM32_I2C_ADDR  0x28


static int              major_num;
static dev_t            dev_file;

static struct class*            char_class = NULL;
static struct device*           char_device = NULL;
static struct i2c_adapter*      stm_adapter = NULL;
static struct i2c_client*       stm_client = NULL;

static uint8_t stm_data[2] = {0};

#if 0

#define     STM_OFF_TIME        _IOW('i', 0, uint8_t)
#define     STM_ON_TIME         _IOW('i', 1, uint8_t)
#define     STM_START_BLINK     _IO ('i', 3)
#define     STM_GET_TIME        _IOR('i', 4, uint8_t*)
#define     STM_GET_DATA        _IOR('i', 5, uint8_t*)

#define     ON_INDEX            0
#define     OFF_INDEX           1
#endif

#define     IOCTL_SET_RTC_TIME          _IOW('i', 0, uint32_t)
#define     IOCTL_SET_PUMPING_TIME      _IOW('i', 1, uint8_t)
#define     IOCTL_SET_PUMP_RUN_TIME     _IOW('i', 2, uint8_t)

#define     IOCTL_GET_RTC_TIME          _IOR('i', 3, uint8_t*)
#define     IOCTL_GET_PUMPING_TIME      _IOR('i', 4, uint8_t*)
#define     IOCTL_GET_PUMP_RUN_TIME     _IOR('i', 5, uint8_t*)
#define     IOCTL_GET_LINE_VOLTAGE      _IOR('i', 6, uint8_t*)

#define     SET_RTC_TIME            0x10
#define     SET_PUMPING_TIME        0x20
#define     SET_PUMP_RUN_TIME       0x30

#define     GET_RTC_TIME            0x80
#define     GET_PUMPING_TIME        0x90
#define     GET_PUMP_RUN_TIME       0xA0
#define     GET_LINE_VOLTAGE        0xB0

static struct i2c_board_info stm_board_info = {
    I2C_BOARD_INFO(DEVICE_NAME, STM32_I2C_ADDR),
};

static void serialize_data(uint8_t cmd, uint32_t data, uint8_t serial_data[]) {
    serial_data[0] = cmd;
    *(uint32_t*)(ser_data+1) = data;
}

static void send_i2c_time(long unsigned int arg, uint8_t cmd) {
    uint32_t tim;
    uint8_t serial_data[8];

    if (copy_from_user(&tim, (uint32_t __user *)arg, 4)) {
        return -EFAULT;
    }

    serialize_data(cmd, tim, serial_data);
    ret = i2c_master_send(stm_client, serial_data, 5);
    if(ret < 0) {
        return -EFAULT;
    }

    pr_info("Sending time command: %02X%02X%02X%02X%02X\n",
	    serial_data[0], serial_data[1], serial_data[2],
	    serial_data[4], serial_data[5]);
}

static long int stm_ioctl(struct file *f, unsigned int cmd,  long unsigned int arg) {
    int ret;

    switch(cmd) {
        case IOCTL_SET_RTC_TIME:
            send_i2c_time(arg, SET_RTC_TIME);
            break;
        case IOCTL_SET_PUMPING_TIME:
            send_i2c_time(arg, SET_PUMPING_TIME);
            break;
        case IOCTL_SET_PUMP_RUN_TIME:
            send_i2c_time(arg, SET_PUMP_RUN_TIME);
            break;

        case IOCTL_GET_RTC_TIME:
            break;
        case IOCTL_GET_PUMPING_TIME:
            break;
        case IOCTL_GET_PUMP_RUN_TIME:
            break;
        case IOCTL_GET_LINE_VOLTAGE:
            break;
    }
#if 0
    uint8_t data[16];

    switch(cmd) {
        case STM_OFF_TIME:
            if (copy_from_user(&stm_data[OFF_INDEX], (uint8_t __user *)arg, 1)) {
			    return -EFAULT;
            }
            break;

        case STM_ON_TIME:
            if (copy_from_user(&stm_data[ON_INDEX], (uint8_t __user *)arg, 1)) {
			    return -EFAULT;
            }
            break;

        case STM_START_BLINK:
            ret = i2c_master_send(stm_client, stm_data, 2);
            if(ret < 0) {
                return -EFAULT;
            }
            pr_info("Sending data to STM: on: %d; off: %d\n",
                stm_data[ON_INDEX], stm_data[OFF_INDEX]);
            break;

        case STM_GET_DATA:
            ret = i2c_master_recv(stm_client, data, 2);
            if(ret < 0) {
                return -EFAULT;
            }
            if(copy_to_user((uint8_t __user*)arg, data, 2)) {
                return -EFAULT;
            }
            break;

        case STM_GET_TIME:
            if(copy_to_user((uint8_t __user*)arg, stm_data, 2)) {
                return -EFAULT;
            }

        default:
            break;
    }
#endif
    return 0;
}

static struct file_operations fops = {
    .unlocked_ioctl = stm_ioctl
};

static int __init simple_drv_readtest_init(void) {
    //
    // STEP 1: General initialization of driver including char device
    // allocation and file operation callback registeration (ioctl)
    //
    major_num = register_chrdev(STATIC_MAJOR, DEVICE_NAME, &fops);
    if( major_num < 0 ) {
        pr_err("Failed to register the device number\n");
        return major_num;
    } else if (major_num == 0) {
        pr_info("Registered device with device number: %d\n", STATIC_MAJOR);
        major_num = STATIC_MAJOR;
    } else {
        pr_info("Device %s registered with major: %d; minor: %d\n",
                DEVICE_NAME, major_num >> 20, major_num & 0xFFFFF);
    }

    // Register class driver
    char_class = class_create(CLASS_NAME);
    if( IS_ERR(char_class)) {
        unregister_chrdev(major_num, DEVICE_NAME);
        return PTR_ERR(char_class);
    }

    pr_info("Class registered successfully; major#: %d\n", major_num);

    // create the device file for the driver
    dev_file = MKDEV(major_num, 0);
    char_device = device_create(char_class, NULL, dev_file,
                                NULL, DEVICE_NAME);
    if( IS_ERR(char_device) ) {
        class_destroy(char_class);
        unregister_chrdev(major_num, DEVICE_NAME);
        pr_err("Failed to create the device\n" );
        return PTR_ERR(char_device);
    }

    //
    // STEP 2: Add I2C device into the i2C subsystem
    //
    stm_adapter = i2c_get_adapter(1);
    if(stm_adapter == NULL) {
        device_destroy(char_class, dev_file);
        class_destroy(char_class);
        unregister_chrdev(major_num, DEVICE_NAME);
        pr_err("Failed to get the i2c adapter\n" );
        return -ENODEV;
    }

    stm_client = i2c_new_client_device(stm_adapter, &stm_board_info);
    if(IS_ERR(stm_client)) {
        device_destroy(char_class, dev_file);
        class_destroy(char_class);
        unregister_chrdev(major_num, DEVICE_NAME);
        pr_err("Failed to create client device\n" );
        return PTR_ERR(char_device);
    }

    i2c_put_adapter(stm_adapter);
    pr_info("Success..!!!! Device driver created successfully\n");
    return 0;
}

static void __exit simple_drv_readtest_exit(void) {
    i2c_unregister_device(stm_client);
    device_destroy(char_class, dev_file);
    class_unregister(char_class);
    class_destroy(char_class);
    unregister_chrdev(major_num, DEVICE_NAME);
    pr_info("Bye kernel\n");
	return;
}

module_init(simple_drv_readtest_init);
module_exit(simple_drv_readtest_exit);

MODULE_DESCRIPTION("This is a simple I2C driver to blink "
                   "the built-in LED of STM32F103C6T6A");
MODULE_AUTHOR("Mohammed Shafeeque");
MODULE_LICENSE("GPL");
