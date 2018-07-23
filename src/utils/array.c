#include <stdlib.h>
#include <string.h>

#include "array.h"

#define ARRAY_P(a, p) ((a)->buffer + (a)->indiv * (p))

array_t *array_new(size_t indiv, size_t cap) {
    array_t *a = (array_t *)malloc(sizeof(array_t));
    a->indiv = indiv;
    a->cap = cap;
    a->alloc = 0;
    a->buffer = malloc(sizeof(array_t) * cap);
    return a;
}

void array_enlarge(array_t *a) {
    a->cap *= 2;

    void *new = malloc(a->indiv * a->cap);
    memcpy(new, a->buffer, a->cap);

    free(a->buffer);
    a->buffer = new;
}

void array_ins(array_t *a, void *e) {
    if ((a->alloc << 1) >= a->cap) {
        array_enlarge(a);
    }

    memcpy(ARRAY_P(a, a->alloc), e, a->indiv);
    a->alloc++;
}

size_t array_rm(array_t *a, array_pos_t s, array_pos_t e) {
    if (a->alloc < e) {
        e = a->alloc;
    }

    size_t n = e - s;
    memcpy(ARRAY_P(a, s), ARRAY_P(a, e), a->alloc - e);
    a->alloc -= n;

    return n;
}

size_t array_size(array_t *a) {
    return a->alloc;
}

void *array_get(array_t *a, array_pos_t p) {
    return ARRAY_P(a, p);
}

void array_free(array_t **pa) {
    free((*pa)->buffer);
    free((*pa));
    (*pa) = NULL;
}
