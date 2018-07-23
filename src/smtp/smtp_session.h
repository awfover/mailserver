#ifndef SMTP_SESSION_H
#define SMTP_SESSION_H

#include "conn.h"
#include "smtp_types.h"

smtp_session_t *smtp_session_new(smtp_t *s, conn_t *c);
void smtp_session_free(smtp_session_t **ss);

int smtp_session_initialized(smtp_session_t *ss);
int smtp_session_in_transaction(smtp_session_t *ss);
int smtp_session_has_recipients(smtp_session_t *ss);
int smtp_session_closed(smtp_session_t *ss);

void smtp_session_state(smtp_session_t *ss, smtp_session_state_enum_t state);
void smtp_session_reset(smtp_session_t *ss);
int smtp_session_greet(smtp_session_t *ss);
int smtp_session_ehlo(smtp_session_t *ss, smtp_cmd_t *sc);
int smtp_session_helo(smtp_session_t *ss, smtp_cmd_t *sc);
int smtp_session_mail(smtp_session_t *ss, smtp_cmd_t *sc);
int smtp_session_rcpt(smtp_session_t *ss, smtp_cmd_t *sc);
int smtp_session_data(smtp_session_t *ss, smtp_cmd_t *sc);
int smtp_session_rset(smtp_session_t *ss, smtp_cmd_t *sc);
int smtp_session_noop(smtp_session_t *ss, smtp_cmd_t *sc);
int smtp_session_quit(smtp_session_t *ss, smtp_cmd_t *sc);

#endif // SMTP_SESSION_H
