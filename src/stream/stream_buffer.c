#include <stdlib.h>
#include <string.h>

#include "stream_buffer.h"
#include "stream_buffer_ticket.h"

stream_buffer_t *stream_buffer_new(size_t id, size_t size) {
    stream_buffer_t *b = (stream_buffer_t *)malloc(sizeof(stream_buffer_t));
    b->id = id;
    b->size = size;
    b->roff = b->woff = 0;
    b->buffer = malloc(size);
    return b;
}

void stream_buffer_free(stream_buffer_t **b) {
    free((*b)->buffer);
    free(*b);
    (*b) = NULL;
}

stream_buffer_ticket_t *stream_buffer_create_rticket(stream_buffer_t *b, size_t n) {
    void *s = b->buffer + b->roff;
    size_t an = b->woff - b->roff;
    if (n < an) {
        an = n;
    }

    return stream_buffer_ticket_new(b, s, an);
}

int stream_buffer_confirm_rticket(stream_buffer_t *b, stream_buffer_ticket_t *t) {
    b->roff += t->n;
    return stream_buffer_rfull(b);
}

int stream_buffer_rfull(stream_buffer_t *b) {
    return ((b->roff == b->woff) ? 1 : 0);
}

stream_buffer_ticket_t *stream_buffer_create_wticket(stream_buffer_t *b, size_t n) {
    void *s = b->buffer + b->woff;
    size_t an = b->size - b->woff;
    if (n < an) {
        an = n;
    }

    return stream_buffer_ticket_new(b, s, an);
}

int stream_buffer_confirm_wticket(stream_buffer_t *b, stream_buffer_ticket_t *t, size_t n) {
    b->woff += n;
    return stream_buffer_wfull(b);
}

int stream_buffer_wfull(stream_buffer_t *b) {
    return ((b->woff == b->size) ? 1 : 0);
}
