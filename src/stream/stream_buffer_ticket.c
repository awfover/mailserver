#include <stdlib.h>

#include "stream_buffer_ticket.h"

stream_buffer_ticket_t *stream_buffer_ticket_new(stream_buffer_t *b, void *s, size_t n) {
    stream_buffer_ticket_t *t = malloc(sizeof(stream_buffer_ticket_t));
    t->b = b;
    t->s = s;
    t->n = n;
    return t;
}

void stream_buffer_ticket_free(stream_buffer_ticket_t **t) {
    free(*t);
    (*t) = NULL;
}
