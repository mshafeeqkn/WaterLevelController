#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define     IOCTL_SET_RTC_TIME          _IOW('i', 0, uint32_t)
#define     IOCTL_SET_PUMPING_TIME      _IOW('i', 1, uint32_t)
#define     IOCTL_SET_PUMP_RUN_TIME     _IOW('i', 2, uint32_t)

#define     IOCTL_GET_RTC_TIME          _IOR('i', 3, uint32_t*)
#define     IOCTL_GET_PUMPING_TIME      _IOR('i', 4, uint32_t*)
#define     IOCTL_GET_PUMP_RUN_TIME     _IOR('i', 5, uint32_t*)
#define     IOCTL_GET_LINE_VOLTAGE      _IOR('i', 6, uint32_t*)

#define     STM_DEVICE_FILE             "/dev/wlc"

static int get_i2c_dev_file() {
    // Open the device file (replace with actual device path)
    int fd = open(STM_DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device");
        return -1;
    }
    return fd;
}

static int stm_send_ioctl(int cmd, uint32_t *data) {
    int fd = get_i2c_dev_file();
    if(fd < 0) {
        return -1;
    }

    int ret = ioctl(fd, cmd, data);
    if (ret < 0) {
        perror("ioctl get state failed");
        return -1;
    }
    close(fd);
    return 0;
}

int stm_load_system_time(uint32_t *sys_time) {
    return stm_send_ioctl(IOCTL_GET_RTC_TIME, sys_time);
}

int stm_set_system_time(uint32_t sys_time) {
    return stm_send_ioctl(IOCTL_SET_RTC_TIME, &sys_time);
}
