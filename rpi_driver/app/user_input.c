#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <ncurses.h>

pthread_t user_input_thread;
extern uint8_t          run_app;
extern pthread_mutex_t  app_mutex;
extern uint32_t         sys_time;
extern uint32_t         pc_time;
extern uint32_t         pumping_time;
extern uint32_t         pump_run_sec;
extern uint32_t         foot_start_x;
extern uint8_t          sync_clock;

extern void print_log(const char *format, ...); 
extern int stm_set_system_time(uint32_t sys_time);
extern int stm_set_pumping_time(uint32_t pumping_time);
extern int stm_set_pump_runtime(uint32_t pump_runtime);
extern int stm_save_to_flash();

void set_alarm_time() {
    char buffer[64];
    echo();  // Enable echoing for user input
    mvprintw(12, foot_start_x+1, "Enter new alarm time in 24hr format (hh:mm:ss): ");
    getstr(buffer);  // Get user input
    noecho();  // Disable echoing

    // Parse the input time
    int hr, min, sec;
    if (sscanf(buffer, "%d:%d:%d", &hr, &min, &sec) == 3) {
        pumping_time = (hr * 3600) + (min * 60) + sec;
    } else {
        mvprintw(12, 0, "Invalid time format.");
    }
    stm_set_pumping_time(pumping_time);
    refresh();  // Refresh to show any changes
}

void set_pumping_time() {
    char buffer[64];
    echo();  // Enable echoing for user input
    mvprintw(12, foot_start_x+1, "Enter the pump run time (sec): ");
    getstr(buffer);  // Get user input
    noecho();  // Disable echoing

    // Parse the input time
    if (sscanf(buffer, "%u", &pump_run_sec) != 1) {
        mvprintw(12, 0, "Invalid number format.");
    }
    stm_set_pump_runtime(pump_run_sec);
    refresh();  // Refresh to show any changes
}

void sync_system_clock() {
    sys_time = pc_time;
    stm_set_system_time(sys_time);
}

void save_to_flash() {
    stm_save_to_flash();
}

void* ncurses_cmd_thread(void *arg) {
    while (1) {
        print_log("Looping command thread\n");
        char ch = getchar();
        pthread_mutex_lock(&app_mutex);
        switch(ch) {
            case 'q':
            case 'Q':
                run_app = 0;
                break;

            case 's':
            case 'S':
                sync_system_clock();
                break;

            case 'a':
            case 'A':
                set_alarm_time();
                break;

            case 'p':
            case 'P':
                set_pumping_time();
                break;

            case 'W':
            case 'w':
                save_to_flash();
                break;
        }
        pthread_mutex_unlock(&app_mutex);
    }
    return NULL;
}

int init_userinput_thread() {
    if (pthread_create(&user_input_thread, NULL, ncurses_cmd_thread, NULL) != 0) {
        perror("Failed to create user input thread");
        return -1;
    }
    
    return 0;
}
