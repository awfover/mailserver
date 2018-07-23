#include <ctype.h>
#include <strings.h>

#include "conn.h"
#include "utils.h"
#include "smtp_cmd.h"
#include "smtp_codes.h"
#include "smtp_cmd_token.h"
#include "smtp_cmd_parser.h"

int smtp_cmd_parse(smtp_cmd_t **cmd, smtp_session_t *ss) {
    int ln = 0;
    int crlf = 0;
    int err = 0;
    char *cmds = ss->cmdln;

    (*cmd) = smtp_cmd_new();
    for (ln; ln < 512; ln++) {
        err = conn_read_ch(ss->c, cmds + ln);
        if (err < 0) {
            return err;
        }
        else if (err == 0) {
            ln--;
        }
        else if ((ln > 2) && iscrlf(cmds + ln - 1)) {
            crlf = 1;
            break;
        }
    }

    if (!crlf) {
        return SMTP_CODE_WRITE(conn_write, ss->c, LINE_TOO_LONG);
    }

    char *cmde = cmds + ln - 1;
    smtp_cmd_enum_t ce = SMTP_CMD_NULL;
    int n = smtp_cmd_parse_name(&ce, cmds, cmde);
    if (n < 0) {
        return SMTP_CODE_WRITE(conn_write, ss->c, COMMAND_UNRECOGNIZED);
    }
    if (smtp_cmd_not_imp(ce)) {
        return SMTP_CODE_WRITE(conn_write, ss->c, COMMAND_NOT_IMPLEMENTED);
    }

    if (ce == SMTP_CMD_EHLO) {
        err = smtp_cmd_parse_ehlo(*cmd, cmds + n, cmde);
    }
    else if (ce == SMTP_CMD_HELO) {
        err = smtp_cmd_parse_helo(*cmd, cmds + n, cmde);
    }
    else if (ce == SMTP_CMD_MAIL) {
        err = smtp_cmd_parse_mail(*cmd, cmds + n, cmde);
    }
    else if (ce == SMTP_CMD_RCPT) {
        err = smtp_cmd_parse_rcpt(*cmd, cmds + n, cmde);
    }
    else if (ce == SMTP_CMD_DATA) {
        err = smtp_cmd_parse_data(*cmd, cmds + n, cmde);
    }
    else if (ce == SMTP_CMD_RSET) {
        err = smtp_cmd_parse_rset(*cmd, cmds + n, cmde);
    }
    else if (ce == SMTP_CMD_NOOP) {
        err = smtp_cmd_parse_noop(*cmd, cmds + n, cmde);
    }
    else if (ce == SMTP_CMD_QUIT) {
        err = smtp_cmd_parse_quit(*cmd, cmds + n, cmde);
    }
    else {
        err = -1;
    }

    if (err < 0) {
        return SMTP_CODE_WRITE(conn_write, ss->c, SYNTAX_ERROR);
    }

    return 0;
}

int smtp_cmd_parse_ehlo(smtp_cmd_t *cmd, const char *s, const char *e) {
    smtp_cmd_token_t *t = smtp_cmd_token_new();
    int n = smtp_cmd_parse_domain_addr(t, s, e);
    if (n < 0) {
        smtp_cmd_token_free(&t);
        return -1;
    }

    n += smtp_cmd_skip_sp(s + n, e);
    if ((s + n) < e) {
        smtp_cmd_token_free(&t);
        return -1;
    }

    smtp_cmd_ehlo(cmd, t);

    return n;
}

int smtp_cmd_parse_helo(smtp_cmd_t *cmd, const char *s, const char *e) {
    smtp_cmd_token_t *t = smtp_cmd_token_new();
    int n = smtp_cmd_parse_domain(t, s, e);
    if (n < 0) {
        smtp_cmd_token_free(&t);
        return -1;
    }

    n += smtp_cmd_skip_sp(s + n, e);
    if ((s + n) < e) {
        smtp_cmd_token_free(&t);
        return -1;
    }

    smtp_cmd_helo(cmd, t);

    return n;
}

int smtp_cmd_parse_mail(smtp_cmd_t *cmd, const char *s, const char *e) {
    static const char MAIL_FROM[] = "FROM:";
    static const size_t MAIL_FROM_SIZE = sizeof(MAIL_FROM) - 1;

    if (strncasecmp(s, MAIL_FROM, MAIL_FROM_SIZE) != 0) {
        return -1;
    }

    int n = MAIL_FROM_SIZE;
    n += smtp_cmd_skip_sp(s + n, e);

    smtp_cmd_token_t *rvrs = smtp_cmd_token_new();
    int an = smtp_cmd_parse_path(rvrs, s + n, e);
    if (an < 0) {
        smtp_cmd_token_free(&rvrs);
        return -1;
    }

    smtp_cmd_mail(cmd, rvrs);

    return (n + an);
}

int smtp_cmd_parse_rcpt(smtp_cmd_t *cmd, const char *s, const char *e) {
    static const char RCPT_TO[] = "TO:";
    static const size_t RCPT_TO_SIZE = sizeof(RCPT_TO) - 1;

    if (strncasecmp(s, RCPT_TO, RCPT_TO_SIZE) != 0) {
        return -1;
    }

    int n = RCPT_TO_SIZE;
    n += smtp_cmd_skip_sp(s + n, e);

    // Postmaster support
    smtp_cmd_token_t *forw = smtp_cmd_token_new();
    int an = smtp_cmd_parse_path(forw, s + n, e);
    if (an < 0) {
        smtp_cmd_token_free(&forw);
        return -1;
    }

    smtp_cmd_rcpt(cmd, forw);

    return (n + an);
}

int smtp_cmd_parse_data(smtp_cmd_t *cmd, const char *s, const char *e) {
    smtp_cmd_data(cmd);
    return 0;
}

int smtp_cmd_parse_rset(smtp_cmd_t *cmd, const char *s, const char *e) {
    smtp_cmd_rset(cmd);
    return 0;
}

int smtp_cmd_parse_noop(smtp_cmd_t *cmd, const char *s, const char *e) {
    smtp_cmd_noop(cmd);
    return 0;
}

int smtp_cmd_parse_quit(smtp_cmd_t *cmd, const char *s, const char *e) {
    smtp_cmd_quit(cmd);
    return 0;
}

int smtp_cmd_skip_sp(const char *s, const char *e) {
    char *sp = (char *)s;
    for (sp; (sp < e) && issp(*sp); sp++);
    return (sp - s);
}

int smtp_cmd_parse_name(smtp_cmd_enum_t *ce, const char *s, const char *e) {
    if ((e - s) < SMTP_CMD_NAME_SIZE) {
        return -1;
    }

    for (size_t i = 0; i < SMTP_CMD_NAMES_SIZE; i++) {
        if (strncasecmp(s, SMTP_CMD_NAMES[i], SMTP_CMD_NAME_SIZE) == 0) {
            int n = smtp_cmd_skip_sp(s + SMTP_CMD_NAME_SIZE, e);
            if (((e - s) == SMTP_CMD_NAME_SIZE) || (n > 0)) {
                (*ce) = SMTP_CMD_EHLO + i;
                return (SMTP_CMD_NAME_SIZE + n);
            }
            return -1;
        }
    }

    return -1;
}

int smtp_cmd_parse_domain_addr(smtp_cmd_token_t *t, const char *s, const char *e) {
    if (islquare(*s)) {
        return smtp_cmd_parse_addr(t, s, e);
    }

    return smtp_cmd_parse_domain(t, s, e);
}

int smtp_cmd_parse_domain(smtp_cmd_token_t *t, const char *s, const char *e) {
    char *c = (char *)s;
    int lan = 0;
    for (c; c <= e; c++) {
        if ((c < e) && isalnum(*c)) {
            lan = 1;
            continue;
        }

        if (c == s) {
            return -1;
        }

        if (!lan) {
            return -1;
        }

        lan = 0;
        if ((c < e) && (ispt(*c) || ishyphen(*c))) {
            continue;
        }

        break;
    }

    smtp_cmd_token_domain(t, s, c);
    return (c - s);
}

int smtp_cmd_parse_addr(smtp_cmd_token_t *t, const char *s, const char *e) {
    if (!islquare(*s)) {
        return -1;
    }

    char *lq = (char *)s;
    char *rq = lq + 1;
    for (rq; (rq < e) && !isrquare(*rq); rq++);
    if (rq >= e) {
        return -1;
    }

    if (isdigit(*(lq + 1))) {
        // ipv4
        int ld = 0;
        int pts = 0;
        char *lpt = lq;
        for (char *c = (lq + 1); c <= rq; c++) {
            if ((c < rq) && isdigit(*c)) {
                if ((c - lpt) > 3) {
                    return -1;
                }
                ld = 1;
                continue;
            }

            if (!ld) {
                return -1;
            }

            if (!islt256(lpt + 1, c - lpt - 1)) {
                return -1;
            }

            ld = 0;
            if ((c < rq) && (pts < 3) && ispt(*c)) {
                pts++;
                continue;
            }

            break;
        }

        smtp_cmd_token_addr_v4(t, lq + 1, rq);
    }

    // ipv6
    // general addr tag

    return (rq - lq + 1);
}

int smtp_cmd_parse_path(smtp_cmd_token_t *t, const char *s, const char *e) {
    if (!islt(*s)) {
        return -1;
    }

    char *lt = (char *)s;
    char *rt = lt + 1;
    for (rt; (rt < e) && !isrt(*rt); rt++);
    if (rt >= e) {
        return -1;
    }

    char *co = lt + 1;
    for (co; (co < rt) && !iscolon(*co); co++);

    int n = 0;
    smtp_cmd_token_t *ds = smtp_cmd_token_new();
    if (co < rt) {
        n = smtp_cmd_parse_domain_list(ds, lt + 1, co);
        if ((n < 0) || ((n != (co - lt - 1)))) {
            smtp_cmd_token_free(&ds);
            return -1;
        }
    }

    char *mbs = (co < rt) ? (co + 1) : (lt + 1);
    smtp_cmd_token_t *mb = smtp_cmd_token_new();
    n = smtp_cmd_parse_mailbox(mb, mbs, rt);
    if ((n < 0) || (n != (rt - mbs))) {
        smtp_cmd_token_free(&ds);
        smtp_cmd_token_free(&mb);
        return -1;
    }

    smtp_cmd_token_path(t, ds, mb);

    return (rt - lt + 1);
}

int smtp_cmd_parse_domain_list(smtp_cmd_token_t *t, const char *s, const char *e) {
    if (s == e) {
        return -1;
    }

    size_t i;
    size_t n = 1;
    char *co = (char *)s;
    for (co; co < e; co++) {
        if (iscomma(*co)) {
            n++;
        }
    }

    int dn = 0;
    char *last = (char *)s - 1;
    smtp_cmd_token_list_t *ds = smtp_cmd_token_list_new(n);
    for (i = 0, co = (char *)s; i < n; i++) {
        for (co; (co < e) && !iscomma(*co); co++);

        dn = smtp_cmd_parse_domain(ds->ts + i, last + 1, co);
        if ((dn < 0) || (dn != (co - last - 1))) {
            smtp_cmd_token_list_free(&ds);
            return -1;
        }

        last = co;
    }

    smtp_cmd_token_domain_list(t, ds);
    return 0;
}

int smtp_cmd_parse_mailbox(smtp_cmd_token_t *t, const char *s, const char *e) {
    smtp_cmd_token_t *lp = smtp_cmd_token_new();
    int n = smtp_cmd_parse_localpart(lp, s, e);
    if (n < 0) {
        smtp_cmd_token_free(&lp);
        return -1;
    }

    if (((s + n) >= e) || !isat(*(s + n))) {
        return -1;
    }

    int sn = n + 1;
    smtp_cmd_token_t *dora = smtp_cmd_token_new();
    n = smtp_cmd_parse_domain_addr(dora, s + sn, e);
    if (n < 0) {
        smtp_cmd_token_free(&lp);
        smtp_cmd_token_free(&dora);
        return -1;
    }

    smtp_cmd_token_mailbox(t, lp, dora);
    return (sn + n);
}

int smtp_cmd_parse_localpart(smtp_cmd_token_t *t, const char *s, const char *e) {
    if (isdquote(*s)) {
        return smtp_cmd_parse_quoted_str(t, s, e);
    }

    return smtp_cmd_parse_dot_str(t, s, e);
}

int smtp_cmd_parse_dot_str(smtp_cmd_token_t *t, const char *s, const char *e) {
    if (s == e) {
        return -1;
    }
    if (!isatext(*s)) {
        return -1;
    }

    char *c = (char *)s + 1;
    for (c; (c < e) && (isatext(*c) || iscolon(*c)); c++);

    smtp_cmd_token_dot_str(t, s, c);
    return (c - s);
}

int smtp_cmd_parse_quoted_str(smtp_cmd_token_t *t, const char *s, const char *e) {
    if ((s == e) || !isdquote(*s)) {
        return -1;
    }

    int lb = 0;
    char *rq = (char *)s + 1;
    for (rq; rq < e; rq++) {
        if (((*rq) < 32) || ((*rq) > 126)) {
            return -1;
        }

        if (!lb && isdquote(*rq)) {
            break;
        }

        lb = isbslash(*rq);
    }

    if ((rq >= e) || !isdquote(*rq)) {
        return -1;
    }

    smtp_cmd_token_quoted_str(t, s, rq + 1);
    return (rq - s + 1);
}
