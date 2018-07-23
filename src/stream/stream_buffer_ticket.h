#ifndef STREAM_BUFFER_TICKET_H
#define STREAM_BUFFER_TICKET_H

#include <sys/types.h>

#include "stream_types.h"

stream_buffer_ticket_t *stream_buffer_ticket_new(stream_buffer_t *b, void *s, size_t n);
void stream_buffer_ticket_free(stream_buffer_ticket_t **t);

#endif // STREAM_BUFFER_TICKET_H
