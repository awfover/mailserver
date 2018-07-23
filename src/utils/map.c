#include <stdlib.h>

#include "map.h"

int map_kv_cmp(void *a, void *b) {
    if (!a || !b) {
        return -1;
    }
    return (((map_kv_t *)a)->k - ((map_kv_t *)b)->k);
}

map_kv_t *map_kv_new(map_key_t k, void *v) {
    map_kv_t *kv = (map_kv_t *)malloc(sizeof(map_kv_t));
    kv->k = k;
    kv->v = v;
    return kv;
}

map_t *map_new(size_t cap) {
    map_t *m = (map_t *)malloc(sizeof(map_t));
    m->cap = cap;
    m->kss = (linked_t **)malloc(sizeof(linked_t *) * cap);

    for (linked_t **ksi = m->kss; ksi < m->kss + m->cap; ksi++) {
        (*ksi) = linked_new();
    }

    return m;
}

void map_set(map_t *m, map_key_t k, void *v) {
    linked_t **ksi = m->kss + (k % m->cap);
    linked_t *l = linked_findf(*ksi, &k, map_kv_cmp);
    map_kv_t *kv = map_kv_new(k, v);
    if (!l) {
        (*ksi) = linked_ins(*ksi, (void *)kv);
    }
    else {
        void *t = linked_mod(l, (void *)kv);
        free(t);
    }
}

void *map_get(map_t *m, map_key_t k) {
    linked_t **ksi = m->kss + (k % m->cap);
    linked_t *l = linked_findf(*ksi, &k, map_kv_cmp);
    void *v = NULL;
    if (l) {
        v = ((map_kv_t *)l->v)->v;
    }
    return v;
}

void map_rm(map_t *m, map_key_t k) {
    linked_t **ksi = m->kss + (k % m->cap);
    linked_t *l = linked_findf(*ksi, &k, map_kv_cmp);
    if (l) {
        (*ksi) = linked_rm(*ksi, l);
    }
}

void map_free(map_t **m) {
    for (linked_t **ksi = (*m)->kss; ksi < (*m)->kss + (*m)->cap; ksi++) {
        for (linked_t *l = linked_head(*ksi); l; l = l->r) {
            free(l->v);
        }
        linked_free(ksi);
    }
    free((*m)->kss);
    free((*m));
    (*m) = NULL;
}
