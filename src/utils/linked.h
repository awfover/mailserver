#ifndef LINKED_H
#define LINKED_H

typedef struct linked {
    void *v;
    struct linked *r;
    struct linked *t;
} linked_t;

linked_t *linked_new();
linked_t *linked_ins(linked_t *l, void *v);
linked_t *linked_rm(linked_t *l, linked_t *x);
linked_t *linked_find(linked_t *l, void *v);
linked_t *linked_findf(linked_t *l, void *v, int cmp(void *, void *));

linked_t *linked_head(linked_t *l);
linked_t *linked_tail(linked_t *l);

int linked_empty(linked_t *l);

void *linked_mod(linked_t *l, void *v);
void linked_free(linked_t **l);

#endif // LINKED_H
