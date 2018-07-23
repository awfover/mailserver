#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include "conn.h"
#include "log.h"
#include "listener.h"

listener_t *listener_new(
    void *sp, int fd, size_t maxevents, size_t stream_bsize,
    conn_t *(*conn_new)(void *v, int fd, struct sockaddr_in *addr)) {
    listener_t *l = (listener_t *)malloc(sizeof(listener_t));
    int epfd = epoll_create1(0);
    if (epfd == -1) {
        log_lib_error("Listener create epoll failed.");
        return l;
    }

    l->fd = fd;
    l->epfd = epfd;
    l->sp = sp;
    l->maxevents = maxevents;
    l->stream_bsize = stream_bsize;
    l->thread = NULL;
    l->conns = map_new(maxevents);

    l->conn_new = conn_new;


    l->mux = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(l->mux, NULL);

    return l;
}

void listener_free(listener_t **l) {
    map_free(&((*l)->conns));
    free((*l)->thread);
    free((*l)->mux);
    free(*l);
    (*l) = NULL;
}

int listener_start(listener_t *l) {
    l->thread = (pthread_t *)malloc(sizeof(pthread_t));

    int err = pthread_create(l->thread, NULL, listener_worker, (void *)l);
    if (err != 0) {
        log_custom_error("Listener create worker failed. Code: %d.", err);
        return -1;
    }

    return 0;
}

int listener_accept(listener_t *l) {
    struct sockaddr_in *addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    socklen_t len;
    int cfd = accept(l->fd, (struct sockaddr *)addr, &len);
    if (cfd == -1) {
        log_lib_error("Listener accept failed.");
        return -1;
    }

    int err = fcntl(cfd, F_SETFL, O_NONBLOCK);
    if (err < 0) {
        log_lib_error("Listener fcntl failed.");
        return -1;
    }

    conn_t *c = l->conn_new(l->sp, cfd, addr);
    err = listener_add_event(l->epfd, cfd, EPOLLIN | EPOLLOUT);
    if (err == -1) {
        log_custom_error("Listener add client events failed. Client fd: %d", cfd);

        conn_free(&c);

        return -1;
    }

    map_set(l->conns, cfd, c);
    return 0;
}

void listener_close_conn(listener_t *l, conn_t *c) {
    pthread_mutex_lock(l->mux);
    map_rm(l->conns, c->fd);
    pthread_mutex_unlock(l->mux);
}

int listener_add_event(int epfd, int fd, int events) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;

    int err = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
    if (err == -1) {
        log_lib_error("Listener add event failed.");
        return -1;
    }

    return 0;
}

void *listener_worker(void *p) {
    listener_t *l = (listener_t *)p;
    int err = listener_add_event(l->epfd, l->fd, EPOLLIN);
    if (err == -1) {
        log_lib_error("Listener worker start failed.");
        return NULL;
    }

    int n, fd;
    conn_t *c;
    struct epoll_event *ev;
    struct epoll_event events[l->maxevents];
    for (;;) {
        n = epoll_wait(l->epfd, events, l->maxevents, -1);
        if (n < 0) {
            log_lib_error("Listener epoll wait error.");
            return NULL;
        }

        for (ev = events; ev < (events + n); ev++) {
            fd = (ev->data).fd;
            if (fd == l->fd) {
                listener_accept(l);
            }
            else {
                pthread_mutex_lock(l->mux);
                c = (conn_t *)map_get(l->conns, fd);
                if (c) {
                    if (ev->events & EPOLLIN) {
                        conn_raw_write(c);
                    }
                    else if (ev->events & EPOLLOUT) {
                        conn_raw_read(c);
                    }
                }
                pthread_mutex_unlock(l->mux);
            }
        }
    }

    return NULL;
}
