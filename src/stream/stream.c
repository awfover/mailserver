#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "stream.h"
#include "stream_buffer.h"
#include "stream_buffer_ticket.h"

stream_t *stream_new(size_t bsize) {
    stream_t *s = (stream_t *)malloc(sizeof(stream_t));
    s->id = 0;
    s->bsize = bsize;
    s->bs = linked_new();
    return s;
}

void stream_free(stream_t **s) {
    stream_buffer_t *b;
    for (linked_t *l = linked_head((*s)->bs); l; l = l->r) {
        b = (stream_buffer_t *)(l->v);
        stream_buffer_free(&b);
    }

    linked_free(&((*s)->bs));
    free(*s);
    (*s) = NULL;
}

stream_buffer_t *stream_alloc_buffer(stream_t *s) {
    stream_buffer_t *b = stream_buffer_new(s->id++, s->bsize);
    s->bs = linked_ins(s->bs, (void *)b);
    return b;
}

stream_buffer_ticket_t *stream_create_rticket(stream_t *s, size_t n) {
    if (!linked_empty(s->bs)) {
        linked_t *l = linked_head(s->bs);
        stream_buffer_t *b = (stream_buffer_t *)(l->v);
        return stream_buffer_create_rticket(b, n);
    }
    return NULL;
}

void stream_confirm_rticket(stream_t *s, stream_buffer_ticket_t *t) {
    int n = stream_buffer_confirm_rticket(t->b, t);
    if (n != 0) {
        stream_buffer_free(&t->b);

        linked_t *l = linked_head(s->bs);
        s->bs = linked_rm(s->bs, l);
    }

    stream_buffer_ticket_free(&t);
}

stream_buffer_ticket_t *stream_create_wticket(stream_t *s, size_t n) {
    stream_buffer_t *b = NULL;
    if (!linked_empty(s->bs)) {
        linked_t *l = linked_head(s->bs);
        b = (stream_buffer_t *)(l->v);
    }
    if (!b || stream_buffer_wfull(b)) {
        b = stream_alloc_buffer(s);
    }
    return stream_buffer_create_wticket(b, n);
}

void stream_confirm_wticket(stream_t *s, stream_buffer_ticket_t *t, size_t n) {
    stream_buffer_confirm_wticket(t->b, t, n);
    stream_buffer_ticket_free(&t);
}

int stream_read_ch(stream_t *s, char *ch) {
    stream_buffer_ticket_t *t = stream_create_rticket(s, 1);
    if (!t) {
        return 0;
    }

    if (t->n > 0) {
        (*ch) = *(char *)(t->s);
    }

    stream_confirm_rticket(s, t);

    return t->n;
}

int stream_read_to(stream_t *s, int fd, int n) {
    int has = 1;
    size_t an = (n == -1) ? s->bsize : n;
    size_t tn = 0;

    int pw = 0;
    size_t tw = 0;

    stream_buffer_ticket_t *t;
    while (has) {
        t = stream_create_rticket(s, an);
        if (!t || (t->n == 0)) {
            has = 0;
            break;
        }

        for (tw = 0; tw < t->n;) {
            pw = write(fd, t->s, t->n - tw);
            if (pw < 0) {
                log_lib_error("Stream write to socket failed.");
                stream_buffer_ticket_free(&t);
                return -1;
            }

            tw += pw;
        }

        tn += t->n;
        if (n >= 0) {
            an -= t->n;
        }

        stream_confirm_rticket(s, t);
    }
    return tn;
}

int stream_write(stream_t *s, const void *v, size_t n) {
    stream_buffer_ticket_t *t;
    for (size_t tw = 0; tw < n;) {
        t = stream_create_wticket(s, n - tw);
        memcpy(t->s, v + tw, t->n);
        tw += t->n;
        stream_confirm_wticket(s, t, t->n);
    }

    return n;
}

int stream_write_from(stream_t *s, int fd, int n) {
    int has = 1;
    size_t an = s->bsize;
    size_t tn = 0;

    int pw = 0;
    size_t tw = 0;

    stream_buffer_ticket_t *t;
    while (has) {
        if ((n >= 0) && ((n - tn) < s->bsize)) {
            an = n - tn;
        }

        t = stream_create_wticket(s, an);
        for (tw = 0; has && (tw < t->n);) {
            pw = read(fd, t->s + tw, t->n - tw);
            if (pw < 0) {
                if (errno == EAGAIN) {
                    pw = 0;
                }
                else {
                    log_lib_error("Stream read from socket failed.");
                    stream_buffer_ticket_free(&t);
                    return -1;
                }
            }
            if (pw == 0) {
                has = 0;
            }
            if (pw > 0) {
                char buff[600];
                memcpy(buff, t->s + tw, pw);
                buff[pw] = '\0';
                log_debug("Read %s", buff);
            }

            tw += pw;
            tn += pw;
        }

        stream_confirm_wticket(s, t, tn);
    }

    return tn;
}

int stream_has(stream_t *s) {
    int has = 0;
    linked_t *l = linked_tail(s->bs);
    if (l) {
        stream_buffer_t *b = (stream_buffer_t *)(l->v);
        has = !stream_buffer_rfull(b);
    }
    return has;
}
