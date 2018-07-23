#include <stdlib.h>
#include <string.h>

#include "linked.h"

linked_t *linked_new() {
    linked_t *l = (linked_t *)malloc(sizeof(linked_t));
    l->v = l->r = l->t = NULL;
    return l;
}

linked_t *linked_ins(linked_t *l, void *v) {
    linked_t *t = linked_new();
    linked_mod(t, v);

    if (linked_empty(l)) {
        l->r = t;
    }
    else {
        l->t->r = t;
    }

    l->t = t;

    return l;
}

linked_t *linked_rm(linked_t *l, linked_t *x) {
    if (!l || !x || (l == x)) {
        return l;
    }

    linked_t *a = NULL;
    linked_t *b = l;
    for (; b && (b != x); (a = b), (b = b->r));
    if (!a || !b) {
        return l;
    }

    a->r = b->r;
    b->r = NULL;
    if (b == l->t) {
        l->t = (l == a) ? NULL : a;
    }

    free(b);

    return l;
}

linked_t *linked_find(linked_t *l, void *v) {
    for (linked_t *i = l; i != NULL; i = i->r) {
        if (i->v && (i->v == v)) {
            return i;
        }
    }
    return NULL;
}

linked_t *linked_findf(linked_t *l, void * v, int cmp(void *, void *)) {
    for (linked_t *i = l; i != NULL; i = i->r) {
        if (i->v && (cmp(i->v, v) == 0)) {
            return i;
        }
    }
    return NULL;
}

linked_t *linked_head(linked_t *l) {
    return l->r;
}

linked_t *linked_tail(linked_t *l) {
    return l->t;
}

int linked_empty(linked_t *l) {
    return ((l->r == NULL) ? 1 : 0);
}

void *linked_mod(linked_t *l, void *v) {
    void *t = l->v;
    l->v = v;
    return t;
}

void linked_free(linked_t **l) {
    linked_t *t;
    while (t = ((*l)->r)) {
        (*l) = linked_rm(*l, t);
    }

    free(*l);
    (*l) = NULL;
}
