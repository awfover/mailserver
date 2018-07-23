#ifndef MAP_H
#define MAP_H

#include "linked.h"

typedef int map_key_t;

typedef struct {
    size_t cap;
    linked_t **kss;
} map_t;

typedef struct {
    map_key_t k;
    void *v;
} map_kv_t;

map_t *map_new(size_t cap);
void map_set(map_t *m, map_key_t k, void *v);
void *map_get(map_t *m, map_key_t k);
void map_rm(map_t *m, map_key_t k);
void map_free(map_t **m);

#endif // MAP_H
