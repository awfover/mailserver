#include <stdlib.h>
#include <pthread.h>

#include "conn.h"
#include "log.h"
#include "linked.h"
#include "smtp_worker.h"
#include "smtp_handler.h"

void smtp_worker_init(smtp_worker_t *w, void *s, size_t id, void (*conn_close)(void *s, conn_t *c)) {
    w->id = id;
    w->s = s;
    w->thread = NULL;
    w->conn_close = conn_close;
}

int smtp_worker_start(smtp_worker_t *w) {
    if (w->thread) {
        log_custom_error("Smtp Worker %d already started.", w->id);
        return -1;
    }

    w->thread = (pthread_t *)malloc(sizeof(pthread_t));
    int err = pthread_create(w->thread, NULL, smtp_worker, (void *)w);
    if (err != 0) {
        log_custom_error("Worker %d start failed. Code: %d.", w->id, err);
        return -1;
    }

    return 0;
}

void *smtp_worker(void *v) {
    smtp_worker_t *w = (smtp_worker_t *)v;
    smtp_t *s = (smtp_t *)w->s;

    for (;;) {
        pthread_mutex_lock(s->mux);
        while (linked_empty(s->conns)) {
            pthread_cond_wait(s->cond, s->mux);
        }

        linked_t *l = linked_head(s->conns);
        conn_t *c = (conn_t *)(l->v);
        smtp_handler(s, c);
        s->conns = linked_rm(s->conns, l);

        pthread_mutex_unlock(s->mux);

        w->conn_close(w->s, c);
    }

    return NULL;
}
