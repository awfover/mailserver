#include <time.h>
#include <stdlib.h>

#include "conn.h"
#include "log.h"

conn_t *conn_new(int fd, struct sockaddr_in *addr, size_t stream_bsize) {
    conn_t *c = (conn_t *)malloc(sizeof(conn_t));
    c->fd = fd;
    c->timeout = -1;
    c->last_read = time(NULL);
    c->addr = addr;
    c->stream_bsize = stream_bsize;
    c->si = stream_new(stream_bsize);
    c->so = stream_new(stream_bsize);

    c->mux = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(c->mux, NULL);

    c->cond_si = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
    pthread_cond_init(c->cond_si, NULL);

    c->cond_so = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
    pthread_cond_init(c->cond_so, NULL);

    return c;
}

void conn_free(conn_t **c) {
    stream_free(&((*c)->si));
    stream_free(&((*c)->so));

    pthread_mutex_destroy((*c)->mux);
    pthread_cond_destroy((*c)->cond_si);
    pthread_cond_destroy((*c)->cond_so);
}

void conn_set_timeout(conn_t *c, int timeout) {
    pthread_mutex_lock(c->mux);
    c->timeout = timeout;
    pthread_mutex_unlock(c->mux);
}

int conn_update_last_read(conn_t *c) {
    time_t now = time(NULL);
    time_t off = now - c->last_read;

    c->last_read = now;
    if ((c->timeout >= 0) && (off >= c->timeout)) {
        return -1;
    }

    return 0;
}

int conn_raw_read(conn_t *c) {
    int err = 0;

    pthread_mutex_lock(c->mux);
    // if (!stream_has(c->so)) {
    //     pthread_cond_wait(c->cond_so, c->mux);
    // }

    if (stream_has(c->so)) {
        int n = stream_read_to(c->so, c->fd, -1);
        if (n < 0) {
            err = -1;
            log_custom_error("Conn write to socket failed.");
        }
    }

    pthread_mutex_unlock(c->mux);

    return err;
}

int conn_raw_write(conn_t *c) {
    int err = 0;

    pthread_mutex_lock(c->mux);

    err = conn_update_last_read(c);
    if (err >= 0) {
        int n = stream_write_from(c->si, c->fd, -1);
        if (n < 0) {
            err = -1;
            log_custom_error("Conn read from socket failed.");
        }
        // else {
        //     log_debug("has cond_si");
        //     pthread_cond_signal(c->cond_si);
        //     log_debug("has cond_si1");
        // }
    }

    pthread_mutex_unlock(c->mux);

    return err;
}

int conn_read_ch(conn_t *c, char *ch) {
    int err = 0;

    pthread_mutex_lock(c->mux);
    // if (!stream_has(c->si)) {
    //     log_debug("wait for cond_si");
    //     pthread_cond_wait(c->cond_si, c->mux);
    //     log_debug("get cond_si");
    // }

    if (stream_has(c->si)) {
        err = stream_read_ch(c->si, ch);
    }

    pthread_mutex_unlock(c->mux);

    return err;
}

int conn_write(conn_t *c, const void *s, size_t n) {
    int err = 0;

    pthread_mutex_lock(c->mux);

    int wn = stream_write(c->so, s, n);
    if (wn < 0) {
        err = -1;
    }
    // else {
    //     pthread_cond_signal(c->cond_so);
    // }

    pthread_mutex_unlock(c->mux);

    return err;
}
