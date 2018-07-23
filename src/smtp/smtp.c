#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>

#include "log.h"
#include "linked.h"
#include "smtp.h"
#include "smtp_worker.h"

smtp_t *smtp_new(smtp_opt_t *opt) {
    smtp_t *s = (smtp_t *)malloc(sizeof(smtp_t));
    s->l = NULL;
    s->opt = opt;
    return s;
}

int smtp_start(smtp_t *s) {
    if (s->started) {
        log_custom_error("Smtp already started.");
        return -1;
    }

    s->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (s->fd == -1) {
        log_lib_error("Smtp create socket failed.");
        return -1;
    }

    int err = setsockopt(s->fd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
    if (err < 0) {
        log_lib_error("Smtp setsockopt failed.");
        return -1;
    }

    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(s->opt->port);

    err = bind(s->fd, (struct sockaddr*)&addr, sizeof(addr));
    if (err == -1) {
        log_lib_error("Smtp bind socket failed.");
        return -1;
    }

    err = listen(s->fd, s->opt->backlog);
    if (err == -1) {
        log_lib_error("Smtp start socket listening for connections failed.");
        return -1;
    }

    s->conns = linked_new();

    s->mux = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(s->mux, NULL);

    s->cond = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
    pthread_cond_init(s->cond, NULL);

    s->workers = (smtp_worker_t *)malloc(sizeof(smtp_worker_t) * s->opt->maxworkers);
    for (size_t i = 0; i < s->opt->maxworkers; i++) {
        smtp_worker_init(s->workers + i, (void *)s, i, smtp_conn_close);
        smtp_worker_start(s->workers + i);
        if (err == -1) {
            log_custom_error("Smtp start worker failed.");
            return -1;
        }
    }

    s->l = listener_new(
        (void *)s,
        s->fd,
        s->opt->maxevents,
        s->opt->stream_bsize,
        smtp_conn_new
    );
    err = listener_start(s->l);
    if (err < 0) {
        return -1;
    }

    s->started = 1;
    return 0;
}

conn_t *smtp_conn_new(void *v, int fd, struct sockaddr_in *addr) {
    smtp_t *s = (smtp_t *)v;
    pthread_mutex_lock(s->mux);
    conn_t *c = conn_new(fd, addr, s->opt->stream_bsize);
    s->conns = linked_ins(s->conns, (void *)c);
    pthread_cond_signal(s->cond);
    pthread_mutex_unlock(s->mux);
    return c;
}

void smtp_conn_close(void *v, conn_t *c) {
    smtp_t *s = (smtp_t *)v;
    listener_close_conn(s->l, c);
    close(c->fd);
    conn_free(&c);
}
