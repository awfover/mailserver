#ifndef LOG_H
#define LOG_H

int log_init(const char *file_s);

void log_debug(const char *fmt, ...);
void log_error(const char *s);
void log_lib_error(const char *s);
void log_custom_error(const char *fmt, ...);

#endif // LOG_H
