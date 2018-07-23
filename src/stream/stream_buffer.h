#ifndef STREAM_BUFFER_H
#define STREAM_BUFFER_H

#include <sys/types.h>

#include "stream_types.h"

stream_buffer_t *stream_buffer_new(size_t id, size_t size);
void stream_buffer_free(stream_buffer_t **b);

stream_buffer_ticket_t *stream_buffer_create_rticket(stream_buffer_t *b, size_t n);
int stream_buffer_confirm_rticket(stream_buffer_t *b, stream_buffer_ticket_t *t);
int stream_buffer_rfull(stream_buffer_t *b);

stream_buffer_ticket_t *stream_buffer_create_wticket(stream_buffer_t *b, size_t n);
int stream_buffer_confirm_wticket(stream_buffer_t *b, stream_buffer_ticket_t *t, size_t n);
int stream_buffer_wfull(stream_buffer_t *b);

#endif // STREAM_BUFFER_H
