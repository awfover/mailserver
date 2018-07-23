#ifndef STREAM_TYPES_H
#define STREAM_TYPES_H

#include <sys/types.h>

#include "linked.h"

typedef struct {
    size_t id;
    size_t bsize;
    linked_t *bs;
} stream_t;

typedef struct {
    size_t id;
    size_t size;
    size_t roff;
    size_t woff;
    void *buffer;
} stream_buffer_t;

typedef struct {
    stream_buffer_t *b;
    void *s;
    size_t n;
} stream_buffer_ticket_t;

#endif // STREAM_TYPES_H
