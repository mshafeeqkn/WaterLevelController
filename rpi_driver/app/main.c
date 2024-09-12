#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdarg.h>


uint8_t          run_app = 1;
pthread_mutex_t  app_mutex;
uint32_t         sys_time;
uint32_t         pc_time;
uint32_t         pumping_time;
uint16_t         voltage;
uint16_t         pump_run_sec;
uint8_t          sync_clock = 0;

pthread_mutex_t run_app_mutex = PTHREAD_MUTEX_INITIALIZER;

extern int init_userinput_thread();
extern pthread_t init_ncurses_gui();
extern int init_backend_thread();
extern int stm_load_system_time(uint32_t *sys_time);
extern int stm_load_pumping_time(uint32_t *pumping_time);

#define DEBUG_APP

void print_log(const char *format, ...) {
#ifdef DEBUG_APP
    FILE *file = fopen("/dev/pts/0", "a"); // Open the file in append mode
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    va_list args;
    va_start(args, format);

    // Write formatted output to the file
    vfprintf(file, format, args);

    va_end(args);
    fclose(file); // Close the file
#endif
}

static int load_stm_data() {
    int ret = 0;
    sys_time = 0;
    pumping_time = 0;
    ret |= stm_load_system_time(&sys_time);
    print_log("system time: %d\n", sys_time);
    ret |= stm_load_pumping_time(&pumping_time);
    print_log("pumping time: %d\n", pumping_time);
    return ret;
}


int main() {
    pthread_t gui_thread;
    int ret;
    time_t tmp_time = time(NULL);

    // Convert the current time to local time representation
    struct tm *local_time = localtime(&tmp_time);
    if (local_time == NULL) {
        perror("localtime");
        return -1;
    }

    pc_time = local_time->tm_sec +
        (local_time->tm_min * 60) +
        (local_time->tm_hour * 3600);

    ret = load_stm_data();
    if(ret < 0) {
        return -1;
    }

    gui_thread = init_ncurses_gui();
    
    if(0 != init_userinput_thread()) {
        print_log("user command initialization failed\n");
        return -1;
    }

    if (0 != init_backend_thread()) {
        perror("Failed to create thread 3");
        return -1;
    }

    if (pthread_join(gui_thread, NULL) != 0) {
            perror("Failed to join thread 2");
            exit(EXIT_FAILURE);
    }
    return 0;
}

