#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>

extern uint8_t          run_app;
extern pthread_mutex_t  app_mutex;
extern uint32_t         sys_time;
extern uint32_t         pc_time;
extern uint32_t         pumping_time;
extern uint16_t         voltage;
extern uint16_t         pump_run_sec;

static pthread_t   backend_thread;
void* thread_backend(void *arg) {
    while (run_app) {
        pthread_mutex_lock(&app_mutex);
        sys_time++;
        pc_time++;
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
