#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>

extern uint8_t          run_app;
extern pthread_mutex_t  app_mutex;
extern uint32_t         sys_time;
extern uint32_t         pc_time;
extern uint32_t         voltage;
extern uint32_t         pumping_time;

static pthread_t   backend_thread;
extern int stm_load_line_voltage(uint32_t *voltage);


void* thread_backend(void *arg) {
    uint8_t rep = 0;
    while (run_app) {
        pthread_mutex_lock(&app_mutex);
        sys_time++;
        pc_time++;
        if(++rep == 5) {
            stm_load_line_voltage(&voltage);
            rep = 0;
        }
        pthread_mutex_unlock(&app_mutex);
        sleep(1);
    }
    return NULL;
}

int init_backend_thread() {
    if (pthread_create(&backend_thread, NULL, thread_backend, NULL) != 0) {
        perror("Failed to create backend thread");
        return -1;
    }
    
    return 0;
}
