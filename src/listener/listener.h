#ifndef LISTENER_H
#define LISTENER_H

#include <pthread.h>

#include "conn.h"
#include "map.h"

typedef struct {
    int epfd;
    int fd;
    void *sp;
    size_t maxevents;
    size_t stream_bsize;
    pthread_t *thread;
    pthread_mutex_t *mux;
    map_t *conns;
    conn_t *(*conn_new)(void *v, int fd, struct sockaddr_in *addr);
} listener_t;

listener_t *listener_new(
    void *sp, int fd, size_t maxevents, size_t stream_bsize,
    conn_t *(*conn_new)(void *v, int fd, struct sockaddr_in *addr));
void listener_free(listener_t **l);

int listener_start(listener_t *l);
int listener_accept(listener_t *l);
void listener_close_conn(listener_t *l, conn_t *c);
int listener_add_event(int epfd, int fd, int events);
void *listener_worker(void *p);


#endif // LISTENER_H
