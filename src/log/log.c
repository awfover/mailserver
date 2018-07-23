#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>

#include "log.h"

#define HINT_DEBUG "DEBUG"
#define HINT_ERROR "ERROR"

int is_debug = 1;

FILE *fout = NULL;
pthread_mutex_t mux;

char time_s[20];
char hint_s[10];
char msg_s[200];

void update_time() {
    static int size = sizeof(time_s);
    time_t t = time(NULL);
    struct tm *lt = localtime(&t);
    size_t n = strftime(time_s, size, "%Y-%m-%d %H:%M:%S", lt);
    time_s[n] = '\0';
}

void log_log(const char *fmt, ...) {
    pthread_mutex_lock(&mux);

    update_time();
    fprintf(fout, "%s %-6s ", time_s, hint_s);

    va_list vl;
    va_start(vl, fmt);
    vfprintf(fout, fmt, vl);
    va_end(vl);

    fprintf(fout, "\n");

    pthread_mutex_unlock(&mux);
}

void log_debug(const char *fmt, ...) {
    if (!is_debug) {
        return;
    }

    va_list vl;
    va_start(vl, fmt);
    vsprintf(msg_s, fmt, vl);
    va_end(vl);

    strcpy(hint_s, HINT_DEBUG);
    log_log("%s", msg_s);
}

void log_error(const char *s) {
    strcpy(hint_s, HINT_ERROR);
    log_log("%s", s);
}

void log_lib_error(const char *s) {
    const char *err_s = strerror(errno);
    sprintf(msg_s, "%s %s", s, err_s);
    log_error(msg_s);
}

void log_custom_error(const char *fmt, ...) {
    va_list vl;
    va_start(vl, fmt);
    vsprintf(msg_s, fmt, vl);
    va_end(vl);

    log_error(msg_s);
}

int log_init(const char *file_s) {
    if (file_s) {
        FILE *f = fopen(file_s, "a+");
        if (!f) {
            log_lib_error("Init log output failed.");
            return -1;
        }
        fout = f;
    }
    else {
        fout = stderr;
    }

    if (pthread_mutex_init(&mux, NULL) < 0) {
        log_custom_error("Init log mutex failed.");
        return -1;
    }

    return 0;
}
