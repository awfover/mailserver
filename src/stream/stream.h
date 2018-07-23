#ifndef STREAM_H
#define STREAM_H

#include "linked.h"
#include "stream_types.h"

stream_t *stream_new(size_t bsize);
void stream_free(stream_t **s);

stream_buffer_t *stream_alloc_buffer(stream_t *s);

stream_buffer_ticket_t *stream_create_rticket(stream_t *s, size_t n);
void stream_confirm_rticket(stream_t *s, stream_buffer_ticket_t *t);

stream_buffer_ticket_t *stream_create_wticket(stream_t *s, size_t n);
void stream_confirm_wticket(stream_t *s, stream_buffer_ticket_t *t, size_t n);

int stream_read_ch(stream_t *s, char *ch);
int stream_read_to(stream_t *s, int fd, int n);

int stream_write(stream_t *s, const void *v, size_t n);
int stream_write_from(stream_t *s, int fd, int n);

int stream_has(stream_t *s);

#endif // STREAM_H
