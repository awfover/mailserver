#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "log.h"
#include "conn.h"
#include "utils.h"
#include "linked.h"
#include "smtp_codes.h"
#include "smtp_session.h"
#include "smtp_cmd_token.h"

smtp_session_t *smtp_session_new(smtp_t *s, conn_t *c) {
    smtp_session_t *ss = (smtp_session_t *)malloc(sizeof(smtp_session_t));
    ss->s = s;
    ss->c = c;
    ss->rvrs_path = NULL;
    ss->forw_path_list = linked_new();
    smtp_session_state(ss, SMTP_SESSION_NULL);

    ss->mailpath = (char *)malloc(sizeof(char) * (strlen(s->opt->mailsdir) + 8));
    sprintf(ss->mailpath, "%s/XXXXXX", s->opt->mailsdir);
    return ss;
}

void smtp_session_free(smtp_session_t **ss) {
    smtp_session_reset(*ss);

    linked_free(&((*ss)->forw_path_list));
    free((*ss)->mailpath);
    free(*ss);
    (*ss) = NULL;
}

int smtp_session_initialized(smtp_session_t *ss) {
    return (ss->state >= SMTP_SESSION_INITIALIZED);
}

int smtp_session_in_transaction(smtp_session_t *ss) {
    return ((ss->state >= SMTP_SESSION_IN_MAIL) && (ss->state <= SMTP_SESSION_IN_DATA));
}

int smtp_session_has_recipients(smtp_session_t *ss) {
    return (ss->state >= SMTP_SESSION_IN_RECIPIENTS);
}

int smtp_session_closed(smtp_session_t *ss) {
    return (ss->state == SMTP_SESSION_CLOSED);
}

void smtp_session_state(smtp_session_t *ss, smtp_session_state_enum_t state) {
    ss->state = state;
    log_debug(
        "Smtp Session changed to state: %s.",
        smtp_session_state_enum_text[state]
    );
}

void smtp_session_reset(smtp_session_t *ss) {
    if (ss->rvrs_path) {
        smtp_cmd_token_free(&(ss->rvrs_path));
    }

    linked_t *l;
    while (!linked_empty(ss->forw_path_list)) {
        l = linked_head(ss->forw_path_list);
        ss->forw_path_list = linked_rm(ss->forw_path_list, l);
    }
}

int smtp_session_greet(smtp_session_t *ss) {
    // const char *gm = ss->s->opt->greeing_msg;
    // int err = conn_write(ss->c, gm, strlen(gm));
    // if (err < 0) {
    //     log_custom_error("Smtp Session greet failed.");
    //     return err;
    // }

    int err = SMTP_CODE_WRITE(conn_write, ss->c, READY);
    if (err < 0) {
        return err;
    }

    smtp_session_state(ss, SMTP_SESSION_GREETED);
    return 0;
}

int smtp_session_ehlo(smtp_session_t *ss, smtp_cmd_t *sc) {
    if (smtp_session_initialized(ss)) {
        return SMTP_CODE_WRITE(conn_write, ss->c, BAD_SEQUENCE_OF_COMMANDS);
    }

    const char EHLO_REPLY[] = "250 mail\n";
    const size_t EHLO_REPLY_SIZE = sizeof(EHLO_REPLY) - 1;

    int err = conn_write(ss->c, EHLO_REPLY, EHLO_REPLY_SIZE);
    if (err < 0) {
        return err;
    }

    smtp_session_state(ss, SMTP_SESSION_INITIALIZED);
    return 0;
}

int smtp_session_helo(smtp_session_t *ss, smtp_cmd_t *sc) {
    if (smtp_session_initialized(ss)) {
        return SMTP_CODE_WRITE(conn_write, ss->c, BAD_SEQUENCE_OF_COMMANDS);
    }

    int err = SMTP_CODE_WRITE(conn_write, ss->c, OK);
    if (err < 0) {
        return err;
    }

    smtp_session_state(ss, SMTP_SESSION_INITIALIZED);
    return 0;
}

int smtp_session_mail(smtp_session_t *ss, smtp_cmd_t *sc) {
    if (!smtp_session_initialized(ss)) {
        return SMTP_CODE_WRITE(conn_write, ss->c, SEND_HELO_FIRST);
    }
    if (smtp_session_in_transaction(ss)) {
        return SMTP_CODE_WRITE(conn_write, ss->c, ALREADY_IN_MAIL_TRANSACTION);
    }

    smtp_cmd_token_copy(&(ss->rvrs_path), sc->t);

    int err = SMTP_CODE_WRITE(conn_write, ss->c, OK);
    if (err < 0) {
        return err;
    }

    smtp_session_state(ss, SMTP_SESSION_IN_MAIL);
    return 0;
}

int smtp_session_rcpt(smtp_session_t *ss, smtp_cmd_t *sc) {
    if (!smtp_session_in_transaction(ss)) {
        return SMTP_CODE_WRITE(conn_write, ss->c, BAD_SEQUENCE_OF_COMMANDS);
    }

    smtp_cmd_token_t *forw_path;
    smtp_cmd_token_copy(&forw_path, sc->t);
    ss->forw_path_list = linked_ins(ss->forw_path_list, forw_path);

    int err = SMTP_CODE_WRITE(conn_write, ss->c, OK);
    if (err < 0) {
        return err;
    }

    smtp_session_state(ss, SMTP_SESSION_IN_RECIPIENTS);
    return 0;
}

int smtp_session_data(smtp_session_t *ss, smtp_cmd_t *sc) {
    if (!smtp_session_has_recipients(ss)) {
        return SMTP_CODE_WRITE(conn_write, ss->c, BAD_SEQUENCE_OF_COMMANDS);
    }

    int err = SMTP_CODE_WRITE(conn_write, ss->c, START_MAIL_INPUT);
    if (err < 0) {
        return err;
    }

    // int64_t ts = unix_timestamp();
    // if (ts < 0) {
    //     return -1;
    // }

    int fd = mkstemp(ss->mailpath);
    if (fd < 0) {
        return -1;
    }

    char ch;
    int s = 0;
    size_t n = 0;
    while (err >= 0) {
        err = conn_read_ch(ss->c, &ch);
        if (err > 0) {
            err = write(fd, (void *)&ch, sizeof(char));
        }
        if (err > 0) {
            n++;
            if (
                (iscr(ch) && ((s == 0) || (s == 3))) ||
                (islf(ch) && ((s == 1) || (s == 4))) ||
                (ispt(ch) && (s == 2))
                ) {
                s++;
            }
            else {
                s = 0;
            }

            if (s == 5) {
                break;
            }
        }
    }

    if (err >= 0) {
        ftruncate(fd, n - 3);
    }
    close(fd);

    if (err < 0) {
        unlink(ss->mailpath);
    }
    else {
        err = SMTP_CODE_WRITE(conn_write, ss->c, QUEUED);
    }

    if (err >= 0) {
        smtp_path_t *p;
        smtp_mailbox_t *mb;

        char *rvrs_buff = (char *)malloc(256);
        p = (smtp_path_t *)(ss->rvrs_path->v);
        mb = (smtp_mailbox_t *)(p->mb->v);
        smtp_mailbox_format(rvrs_buff, mb);

        char *forw_buff = (char *)malloc(256);
        linked_t *l = linked_head(ss->forw_path_list);
        smtp_cmd_token_t *forw_path = (smtp_cmd_token_t *)(l->v);
        p = (smtp_path_t *)(forw_path->v);
        mb = (smtp_mailbox_t *)(p->mb->v);
        smtp_mailbox_format(forw_buff, mb);

        log_debug("\nMail from: %s\nRcpt To: %s\nMail data saved in %s", rvrs_buff, forw_buff, ss->mailpath);

        free(rvrs_buff);
        free(forw_buff);
    }

    return err;
}

int smtp_session_rset(smtp_session_t *ss, smtp_cmd_t *sc) {
    smtp_session_reset(ss);
    smtp_session_state(ss, smtp_session_initialized(ss) ? SMTP_SESSION_INITIALIZED : SMTP_SESSION_GREETED);
    return SMTP_CODE_WRITE(conn_write, ss->c, OK);
}

int smtp_session_noop(smtp_session_t *ss, smtp_cmd_t *sc) {
    return SMTP_CODE_WRITE(conn_write, ss->c, OK);
}

int smtp_session_quit(smtp_session_t *ss, smtp_cmd_t *sc) {
    int err = SMTP_CODE_WRITE(conn_write, ss->c, BYE);
    if (err < 0) {
        return err;
    }

    smtp_session_state(ss, SMTP_SESSION_CLOSED);
    return 0;
}
