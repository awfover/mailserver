#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "utils.h"
#include "smtp_cmd_token.h"

smtp_cmd_token_t *smtp_cmd_token_new() {
    smtp_cmd_token_t *t = (smtp_cmd_token_t *)malloc(sizeof(smtp_cmd_token_t));
    smtp_cmd_token_null(t);
    return t;
}

void smtp_cmd_token_copy(smtp_cmd_token_t **d, smtp_cmd_token_t *s) {
    (*d) = (smtp_cmd_token_t *)malloc(sizeof(smtp_cmd_token_t));
    smtp_cmd_token_copy_without_new(*d, s);
}

void smtp_cmd_token_copy_without_new(smtp_cmd_token_t *d, smtp_cmd_token_t *s) {
    d->k = s->k;
    if (d->k == SMTP_CMD_TOKEN_DOMAIN) {
        string_copy((char **)&(d->v), (char *)s->v);
    }
    else if (d->k == SMTP_CMD_TOKEN_ADDR_V4) {
        string_copy((char **)&(d->v), (char *)s->v);
    }
    else if (d->k == SMTP_CMD_TOKEN_PATH) {
        smtp_path_copy((smtp_path_t **)&(d->v), (smtp_path_t *)s->v);
    }
    else if (d->k == SMTP_CMD_TOKEN_DOMAIN_LIST) {
        smtp_cmd_token_list_copy((smtp_cmd_token_list_t **)&(d->v), (smtp_cmd_token_list_t *)s->v);
    }
    else if (d->k == SMTP_CMD_TOKEN_MAILBOX) {
        smtp_mailbox_copy((smtp_mailbox_t **)&(d->v), (smtp_mailbox_t *)s->v);
    }
    else if (d->k == SMTP_CMD_TOKEN_DOT_STR) {
        string_copy((char **)&(d->v), (char *)s->v);
    }
    else if (d->k == SMTP_CMD_TOKEN_QUOTED_STR) {
        string_copy((char **)&(d->v), (char *)s->v);
    }
}

void smtp_cmd_token_free(smtp_cmd_token_t **t) {
    smtp_cmd_token_enum_t k = (*t)->k;
    void *v = (*t)->v;
    if (k == SMTP_CMD_TOKEN_DOMAIN) {
        string_free((char **)&v);
    }
    else if (k == SMTP_CMD_TOKEN_ADDR_V4) {
        string_free((char **)&v);
    }
    else if (k == SMTP_CMD_TOKEN_PATH) {
        smtp_path_free((smtp_path_t **)&v);
    }
    else if (k == SMTP_CMD_TOKEN_DOMAIN_LIST) {
        smtp_cmd_token_list_free((smtp_cmd_token_list_t **)&v);
    }
    else if (k == SMTP_CMD_TOKEN_MAILBOX) {
        smtp_mailbox_free((smtp_mailbox_t **)&v);
    }
    else if (k == SMTP_CMD_TOKEN_DOT_STR) {
        string_free((char **)&v);
    }
    else if (k == SMTP_CMD_TOKEN_QUOTED_STR) {
        string_free((char **)&v);
    }

    free(*t);
    (*t) =  NULL;
}

smtp_cmd_token_list_t *smtp_cmd_token_list_new(size_t n) {
    smtp_cmd_token_list_t *tl = (smtp_cmd_token_list_t *)malloc(sizeof(smtp_cmd_token_list_t));
    tl->n = n;
    tl->ts = (smtp_cmd_token_t *)malloc(sizeof(smtp_cmd_token_t) * n);
    for (smtp_cmd_token_t *t = tl->ts; t < (tl->ts + n); t++) {
        smtp_cmd_token_null(t);
    }
    return tl;
}

void smtp_cmd_token_list_copy(smtp_cmd_token_list_t **d, smtp_cmd_token_list_t *s) {
    (*d) = (smtp_cmd_token_list_t *)malloc(sizeof(smtp_cmd_token_list_t));
    (*d)->n = s->n;
    (*d)->ts = (smtp_cmd_token_t *)malloc(sizeof(smtp_cmd_token_t) * s->n);
    for (size_t i = 0; i < s->n; i++) {
        smtp_cmd_token_copy_without_new((*d)->ts + i, s->ts + i);
    }
}

void smtp_cmd_token_list_free(smtp_cmd_token_list_t **tl) {
    smtp_cmd_token_t *t;
    for (size_t i = 0; i < (*tl)->n; i++) {
        t = (*tl)->ts + i;
        smtp_cmd_token_free(&t);
    }

    free((*tl)->ts);
    free(*tl);
    (*tl) = NULL;
}

void smtp_cmd_token_null(smtp_cmd_token_t *t) {
    t->k = SMTP_CMD_TOKEN_NULL;
    t->v = NULL;
}

void smtp_cmd_token_domain(smtp_cmd_token_t *t, const char *s, const char *e) {
    t->k = SMTP_CMD_TOKEN_DOMAIN;
    t->v = (void *)string_new(s, e);
}

void smtp_cmd_token_addr_v4(smtp_cmd_token_t *t, const char *s, const char *e) {
    t->k = SMTP_CMD_TOKEN_ADDR_V4;
    t->v = (void *)string_new(s, e);
}

void smtp_cmd_token_path(smtp_cmd_token_t *t, smtp_cmd_token_t *ds, smtp_cmd_token_t *mb) {
    t->k = SMTP_CMD_TOKEN_PATH;
    t->v = (void *)smtp_path_new(ds, mb);
}

void smtp_cmd_token_domain_list(smtp_cmd_token_t *t, smtp_cmd_token_list_t *ts) {
    t->k = SMTP_CMD_TOKEN_DOMAIN_LIST;
    t->v = (void *)ts;
}

void smtp_cmd_token_mailbox(smtp_cmd_token_t *t, smtp_cmd_token_t *lp, smtp_cmd_token_t *droa) {
    t->k = SMTP_CMD_TOKEN_MAILBOX;
    t->v = (void *)smtp_mailbox_new(lp, droa);
}

void smtp_cmd_token_dot_str(smtp_cmd_token_t *t, const char *s, const char *e) {
    t->k = SMTP_CMD_TOKEN_DOT_STR;
    t->v = (void *)string_new(s, e);
}

void smtp_cmd_token_quoted_str(smtp_cmd_token_t *t, const char *s, const char *e) {
    t->k = SMTP_CMD_TOKEN_QUOTED_STR;
    t->v = (void *)string_new(s, e);
}

smtp_path_t *smtp_path_new(smtp_cmd_token_t *ds, smtp_cmd_token_t *mb) {
    smtp_path_t *p = (smtp_path_t *)malloc(sizeof(smtp_path_t));
    p->ds = ds;
    p->mb = mb;
    return p;
}

void smtp_path_copy(smtp_path_t **d, smtp_path_t *s) {
    (*d) = (smtp_path_t *)malloc(sizeof(smtp_path_t));
    smtp_cmd_token_copy(&((*d)->ds), s->ds);
    smtp_cmd_token_copy(&((*d)->mb), s->mb);
}

void smtp_path_free(smtp_path_t **p) {
    smtp_cmd_token_free(&((*p)->ds));
    smtp_cmd_token_free(&((*p)->mb));
    free(*p);
    (*p) = NULL;
}

smtp_mailbox_t *smtp_mailbox_new(smtp_cmd_token_t *lp, smtp_cmd_token_t *droa) {
    smtp_mailbox_t *mb = (smtp_mailbox_t *)malloc(sizeof(smtp_mailbox_t));
    mb->lp = lp;
    mb->droa = droa;
    return mb;
}

void smtp_mailbox_copy(smtp_mailbox_t **d, smtp_mailbox_t *s) {
    (*d) = (smtp_mailbox_t *)malloc(sizeof(smtp_mailbox_t));
    smtp_cmd_token_copy(&((*d)->lp), s->lp);
    smtp_cmd_token_copy(&((*d)->droa), s->droa);
}

void smtp_mailbox_format(char *d, smtp_mailbox_t *s) {
    sprintf(d, "%s@%s", (char *)s->lp->v, (char *)s->droa->v);
}

void smtp_mailbox_free(smtp_mailbox_t **mb) {
    smtp_cmd_token_free(&((*mb)->lp));
    smtp_cmd_token_free(&((*mb)->droa));
    free(*mb);
    (*mb) = NULL;
}
