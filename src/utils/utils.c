#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#include "utils.h"

char *string_new(const char *s, const char *e) {
    size_t n = e - s;
    char *str = (char *)malloc(sizeof(char) * (n + 1));
    memcpy(str, s, n);
    str[n] = '\0';
    return str;
}

void string_copy(char **d, char *s) {
    size_t n = strlen(s);
    (*d) = (char *)malloc(sizeof(char) * (n + 1));
    memcpy(*d, s, n + 1);
}

void string_free(char **str) {
    free(*str);
    (*str) = 0;
}

int64_t unix_timestamp() {
    struct timespec tms;
    if (!timespec_get(&tms, TIME_UTC)) {
        return -1;
    }

    int64_t micros = tms.tv_sec * 1000000;
    micros += tms.tv_nsec/1000;
    if (tms.tv_nsec % 1000 >= 500) {
        ++micros;
    }

    return micros;
}
