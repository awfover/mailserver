#ifndef SMTP_H
#define SMTP_H

#include <netinet/in.h>

#include "conn.h"
#include "smtp_types.h"

smtp_t *smtp_new(smtp_opt_t *opt);
void smtp_free(smtp_t **s);

int smtp_start(smtp_t *s);

conn_t *smtp_conn_new(void *v, int fd, struct sockaddr_in *addr);
void smtp_conn_close(void *v, conn_t *c);

#endif // SMTP_H
