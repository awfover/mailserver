#ifndef SMTP_CMD_TOKEN_H
#define SMTP_CMD_TOKEN_H

#include "smtp_types.h"

smtp_cmd_token_t *smtp_cmd_token_new();
void smtp_cmd_token_copy(smtp_cmd_token_t **d, smtp_cmd_token_t *s);
void smtp_cmd_token_copy_without_new(smtp_cmd_token_t *d, smtp_cmd_token_t *s);
void smtp_cmd_token_free(smtp_cmd_token_t **t);

smtp_cmd_token_list_t *smtp_cmd_token_list_new(size_t n);
void smtp_cmd_token_list_copy(smtp_cmd_token_list_t **d, smtp_cmd_token_list_t *s);
void smtp_cmd_token_list_free(smtp_cmd_token_list_t **tl);

void smtp_cmd_token_null(smtp_cmd_token_t *t);
void smtp_cmd_token_domain(smtp_cmd_token_t *t, const char *s, const char *e);
void smtp_cmd_token_addr_v4(smtp_cmd_token_t *t, const char *s, const char *e);
void smtp_cmd_token_path(smtp_cmd_token_t *t, smtp_cmd_token_t *ds, smtp_cmd_token_t *mb);
void smtp_cmd_token_domain_list(smtp_cmd_token_t *t, smtp_cmd_token_list_t *ts);
void smtp_cmd_token_mailbox(smtp_cmd_token_t *t, smtp_cmd_token_t *lp, smtp_cmd_token_t *droa);
void smtp_cmd_token_dot_str(smtp_cmd_token_t *t, const char *s, const char *e);
void smtp_cmd_token_quoted_str(smtp_cmd_token_t *t, const char *s, const char *e);

smtp_path_t *smtp_path_new(smtp_cmd_token_t *ds, smtp_cmd_token_t *mb);
void smtp_path_copy(smtp_path_t **d, smtp_path_t *s);
void smtp_path_free(smtp_path_t **p);

smtp_mailbox_t *smtp_mailbox_new(smtp_cmd_token_t *lp, smtp_cmd_token_t *droa);
void smtp_mailbox_copy(smtp_mailbox_t **d, smtp_mailbox_t *s);
void smtp_mailbox_format(char *d, smtp_mailbox_t *s);
void smtp_mailbox_free(smtp_mailbox_t **mb);

#endif // SMTP_CMD_TOKEN_H
