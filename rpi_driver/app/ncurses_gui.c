#include <pthread.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

pthread_t gui_thread;
extern uint8_t          run_app;
extern pthread_mutex_t  app_mutex;
extern uint32_t         sys_time;
extern uint32_t         pc_time;
extern uint32_t         pumping_time;
extern uint32_t         voltage;
extern uint32_t         pump_run_sec;

int foot_start_x;
extern void print_log(const char *format, ...); 


static void get_time_string(char *buff, uint32_t time) {
    uint8_t sec = time % 60;
    time /= 60;
    uint8_t min = time % 60;
    time /= 60;
    uint8_t hr = time % 24;
    sprintf(buff, "%02d:%02d:%02d %s", hr%12, min, sec, hr>12?"PM":"AM");
}

static void* ncurses_ui_thread(void *arg) {
    char buff[64] = {0};
    const char *footer = " Q: Quit  S: Sync time  A: pumping time  P: pump run time  W: Save ";
    const int foot_len = strlen(footer);
    int diff = 0;

    // Initialize ncurses
    initscr();              // Start curses mode
    cbreak();               // Line buffering disabled, Pass all input to the program
    noecho();               // Don't echo while we do getch
    curs_set(FALSE);        // Hide the cursor

    int screen_width = getmaxx(stdscr);
    foot_start_x = (screen_width - foot_len) / 2;

    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
        return NULL;
    }
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);

    while (run_app) {
        pthread_mutex_lock(&app_mutex);
        clear();            // Clear the screen
        get_time_string(buff, sys_time);
        diff = pc_time - sys_time;
        mvprintw(5, foot_start_x + 20, " System Time     : %s (%d)", buff, diff);  // Print the counter
        get_time_string(buff, pumping_time);
        mvprintw(6, foot_start_x + 20, " Pumping Time    : %s", buff);  // Print the counter
        mvprintw(7, foot_start_x + 20, " Current Voltage : %dv", voltage);  // Print the counter
        mvprintw(8, foot_start_x + 20, " Pump Run Time   : %d sec", pump_run_sec);  // Print the counter

        attron(COLOR_PAIR(1));
        mvprintw(14, foot_start_x, footer);  // Print the counter
        attroff(COLOR_PAIR(1));  // Turn off the color pair

        refresh();          // Refresh the screen to show changes
        pthread_mutex_unlock(&app_mutex);
        usleep(100000);    // Sleep for 100 ms
    }

    // End ncurses mode
    endwin();
    return NULL;
}

pthread_t init_ncurses_gui() {
    if (pthread_create(&gui_thread, NULL, ncurses_ui_thread, NULL) != 0) {
        perror("Failed to create UI thread");
        return 0;
    }

    return gui_thread;
}
