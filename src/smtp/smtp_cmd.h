#ifndef SMTP_CMD_H
#define SMTP_CMD_H

#include "smtp_types.h"

smtp_cmd_t *smtp_cmd_new();
void smtp_cmd_free(smtp_cmd_t **c);

void smtp_cmd_ehlo(smtp_cmd_t *c, smtp_cmd_token_t *t);
void smtp_cmd_helo(smtp_cmd_t *c, smtp_cmd_token_t *t);
void smtp_cmd_mail(smtp_cmd_t *c, smtp_cmd_token_t *t);
void smtp_cmd_rcpt(smtp_cmd_t *c, smtp_cmd_token_t *t);
void smtp_cmd_data(smtp_cmd_t *c);
void smtp_cmd_rset(smtp_cmd_t *c);
void smtp_cmd_noop(smtp_cmd_t *c);
void smtp_cmd_quit(smtp_cmd_t *c);

#endif // SMTP_CMD_H
