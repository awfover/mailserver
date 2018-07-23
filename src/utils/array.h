#ifndef ARRAY_H
#define ARRAY_H

#include <sys/types.h>

typedef struct {
    size_t indiv;
    size_t cap;
    size_t alloc;
    void *buffer;
} array_t;

typedef size_t array_pos_t;

array_t *array_new(size_t indiv, size_t cap);
void array_ins(array_t *a, void *e);
size_t array_rm(array_t *a, array_pos_t s, array_pos_t e);
void *array_get(array_t *a, array_pos_t p);
size_t array_size(array_t *a);
void array_free(array_t **pa);

#endif // ARRAY_H
