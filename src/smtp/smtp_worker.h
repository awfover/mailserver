#ifndef SMTP_WORKER_H
#define SMTP_WORKER_H

#include "smtp_types.h"

void smtp_worker_init(smtp_worker_t *w, void *s, size_t id, void (*conn_close)(void *s, conn_t *c));
int smtp_worker_start(smtp_worker_t *w);

void *smtp_worker(void *v);

#endif // SMTP_WORKER_H
