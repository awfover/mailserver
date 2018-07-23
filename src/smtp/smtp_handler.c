#include "log.h"
#include "smtp_cmd.h"
#include "smtp_session.h"
#include "smtp_handler.h"
#include "smtp_cmd_parser.h"

void smtp_handler(smtp_t *s, conn_t *c) {
    smtp_session_t *ss = smtp_session_new(s, c);
    smtp_cmd_t *sc = NULL;

    int err = smtp_session_greet(ss);
    while ((err >= 0) && !smtp_session_closed(ss)) {
        sc = NULL;
        err = smtp_cmd_parse(&sc, ss);
        if (err >= 0) {
            if (sc->k == SMTP_CMD_EHLO) {
                err = smtp_session_ehlo(ss, sc);
            }
            else if (sc->k == SMTP_CMD_HELO) {
                err = smtp_session_helo(ss, sc);
            }
            else if (sc->k == SMTP_CMD_MAIL) {
                err = smtp_session_mail(ss, sc);
            }
            else if (sc->k == SMTP_CMD_RCPT) {
                err = smtp_session_rcpt(ss, sc);
            }
            else if (sc->k == SMTP_CMD_DATA) {
                err = smtp_session_data(ss, sc);
            }
            else if (sc->k == SMTP_CMD_RSET) {
                err = smtp_session_rset(ss, sc);
            }
            else if (sc->k == SMTP_CMD_NOOP) {
                err = smtp_session_noop(ss, sc);
            }
            else if (sc->k == SMTP_CMD_QUIT) {
                err = smtp_session_quit(ss, sc);
            }
        }

        if (sc) {
            smtp_cmd_free(&sc);
        }
    }

    smtp_session_free(&ss);
}
