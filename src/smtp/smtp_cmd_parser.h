#ifndef SMTP_CMD_PARSER_H
#define SMTP_CMD_PARSER_H

#include "smtp_types.h"

#define smtp_cmd_not_imp(ce) ((ce) > SMTP_CMD_NOT_IMP)

int smtp_cmd_parse(smtp_cmd_t **cmd, smtp_session_t *ss);
int smtp_cmd_parse_ehlo(smtp_cmd_t *cmd, const char *s, const char *e);
int smtp_cmd_parse_helo(smtp_cmd_t *cmd, const char *s, const char *e);
int smtp_cmd_parse_mail(smtp_cmd_t *cmd, const char *s, const char *e);
int smtp_cmd_parse_rcpt(smtp_cmd_t *cmd, const char *s, const char *e);
int smtp_cmd_parse_data(smtp_cmd_t *cmd, const char *s, const char *e);
int smtp_cmd_parse_rset(smtp_cmd_t *cmd, const char *s, const char *e);
int smtp_cmd_parse_noop(smtp_cmd_t *cmd, const char *s, const char *e);
int smtp_cmd_parse_quit(smtp_cmd_t *cmd, const char *s, const char *e);

int smtp_cmd_skip_sp(const char *s, const char *e);

int smtp_cmd_parse_name(smtp_cmd_enum_t *ce, const char *s, const char *e);
int smtp_cmd_parse_domain_addr(smtp_cmd_token_t *t, const char *s, const char *e);
int smtp_cmd_parse_domain(smtp_cmd_token_t *t, const char *s, const char *e);
int smtp_cmd_parse_addr(smtp_cmd_token_t *t, const char *s, const char *e);
int smtp_cmd_parse_path(smtp_cmd_token_t *t, const char *s, const char *e);
int smtp_cmd_parse_domain_list(smtp_cmd_token_t *t, const char *s, const char *e);
int smtp_cmd_parse_mailbox(smtp_cmd_token_t *t, const char *s, const char *e);
int smtp_cmd_parse_localpart(smtp_cmd_token_t *t, const char *s, const char *e);
int smtp_cmd_parse_dot_str(smtp_cmd_token_t *t, const char *s, const char *e);
int smtp_cmd_parse_quoted_str(smtp_cmd_token_t *t, const char *s, const char *e);

#endif // SMTP_CMD_PARSER_H
