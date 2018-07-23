#include <stdlib.h>

#include "smtp_cmd.h"
#include "smtp_cmd_token.h"

smtp_cmd_t *smtp_cmd_new() {
    smtp_cmd_t *c = (smtp_cmd_t *)malloc(sizeof(smtp_cmd_t));
    c->k = SMTP_CMD_NULL;
    c->t = NULL;
    return c;
}

void smtp_cmd_free(smtp_cmd_t **c) {
    if ((*c)->t) {
        smtp_cmd_token_free(&((*c)->t));
    }

    free(*c);
    (*c) = NULL;
}

void smtp_cmd_ehlo(smtp_cmd_t *c, smtp_cmd_token_t *t) {
    c->k = SMTP_CMD_EHLO;
    c->t = t;
}

void smtp_cmd_helo(smtp_cmd_t *c, smtp_cmd_token_t *t) {
    c->k = SMTP_CMD_HELO;
    c->t = t;
}

void smtp_cmd_mail(smtp_cmd_t *c, smtp_cmd_token_t *t) {
    c->k = SMTP_CMD_MAIL;
    c->t = t;
}

void smtp_cmd_rcpt(smtp_cmd_t *c, smtp_cmd_token_t *t) {
    c->k = SMTP_CMD_RCPT;
    c->t = t;
}

void smtp_cmd_data(smtp_cmd_t *c) {
    c->k = SMTP_CMD_DATA;
}

void smtp_cmd_rset(smtp_cmd_t *c) {
    c->k = SMTP_CMD_RSET;
}

void smtp_cmd_noop(smtp_cmd_t *c) {
    c->k = SMTP_CMD_NOOP;
}

void smtp_cmd_quit(smtp_cmd_t *c) {
    c->k = SMTP_CMD_QUIT;
}
