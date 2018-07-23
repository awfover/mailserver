#ifndef CONN_H
#define CONN_H

#include <pthread.h>
#include <netinet/in.h>
#include <sys/types.h>

#include "stream.h"

typedef struct {
    int fd;
    int timeout;
    time_t last_read;
    struct sockaddr_in *addr;
    size_t stream_bsize;
    stream_t *si;
    stream_t *so;
    pthread_mutex_t *mux;
    pthread_cond_t *cond_si;
    pthread_cond_t *cond_so;
} conn_t;

conn_t *conn_new(int fd, struct sockaddr_in *addr, size_t stream_bsize);
void conn_free(conn_t **c);

void conn_set_timeout(conn_t *c, int timeout);
int conn_update_last_read(conn_t *c);

int conn_raw_read(conn_t *c);
int conn_raw_write(conn_t *c);

int conn_read_ch(conn_t *c, char *ch);
int conn_write(conn_t *c, const void *s, size_t n);

#endif // CONN_H
