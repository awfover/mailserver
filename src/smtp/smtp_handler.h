#ifndef SMTP_HANDLER_H
#define SMTP_HANDLER_H

#include "smtp_types.h"

void smtp_handler(smtp_t *s, conn_t *c);

#endif // SMTP_HANDLER_H
