#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <time.h>

#include "array.h"
#include "linked.h"
#include "map.h"

#define test(fn) \
    test_##fn(); \
    printf("        \e[92m✓ \e[90m%s\e[0m\n", #fn);

#define suite(title) \
    printf("\n    \e[36m%s\e[0m\n", #title)

void test_array() {
    int d[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    array_t *a = array_new(sizeof(int), 4);
    assert(a);
    assert(a->indiv == sizeof(int));
    assert(a->cap == 4);
    assert(a->alloc == 0);
    assert(a->buffer);
    assert(array_size(a) == 0);

    for (int i = 0; i < 10; i++) {
        array_ins(a, &d[i]);
        assert(array_size(a) == (i + 1));
    }

    for (int i = 0; i < 10; i++) {
        assert(*((int *)array_get(a, i)) == d[i]);
    }

    int n = array_rm(a, 0, 1);
    assert((n == 1) && (array_size(a) == 9));
    assert(*((int *)array_get(a, 0)) == d[1]);

    n = array_rm(a, 8, 9);
    assert((n == 1) && (array_size(a) == 8));

    array_ins(a, &d[9]);
    n == array_rm(a, 8, 10);
    assert((n == 1) && (array_size(a) == 8));

    array_free(&a);
    assert(!a);
}

int linked_cmp_int(void *a, void *b) {
    return (*((int *)a) - *((int *)b));
}

void test_linked() {
    int d[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    linked_t *l = linked_new();
    linked_t *t = NULL;
    assert(l);
    assert(!(l->v));
    assert(!(l->r));

    for (int i = 0; i < 5; i++) {
        l = linked_ins(l, (void *)(d + i));
        assert((int *)l->t->v == (d + i));
    }

    t = linked_find(l, (void *)d);
    assert(t);
    assert((int *)t->v == d);

    t = linked_find(l, (void *)(d + 6));
    assert(!t);

    t = linked_find(l, (void *)(d + 4));
    assert(t);
    assert((int *)t->v == (d + 4));

    t = linked_findf(l, (void *)d, linked_cmp_int);
    assert(t);
    assert((int *)t->v == d);

    t = linked_findf(l, (void *)(d + 6), linked_cmp_int);
    assert(!t);

    t = linked_findf(l, (void *)(d + 4), linked_cmp_int);
    assert(t);
    assert((int *)t->v == (d + 4));

    l = linked_rm(l, l);
    assert(!(l->v));

    for (int i = 0; i < 5; i++) {
        l = linked_rm(l, linked_find(l, (void *)(d + i)));
    }
    assert(l->v == NULL);

    for (int i = 0; i < 5; i++) {
        l = linked_ins(l, (void *)(d + i));
        t = linked_find(l, (void *)(d + i));
        assert((int *)t->v == (d + i));
    }

    linked_free(&l);
    assert(!l);
}

void test_map() {
    int d[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    int *t = NULL;
    map_t *m = map_new(5);
    assert(m);
    assert(m->cap == 5);
    assert(m->kss);

    for (int i = 0; i < 9; i++) {
        map_set(m, i, (void *)(d + i));
        t = (int *)map_get(m, i);
        assert(t == (d + i));
    }

    for (int i = 8; i >= 0; i--) {
        t = (int *)map_get(m, i);
        assert(t == (d + i));
    }

    for (int i = 0; i < 9; i++) {
        map_set(m, i, (void *)(d + 8 - i));
        t = (int *)map_get(m, i);
        assert(t == (d + 8 - i));
    }

    for (int i = 0; i < 9; i++) {
        map_rm(m, i);
        t = (int *)map_get(m, i);
        assert(!t);
    }

    for (int i = 0; i < 16; i++) {
        map_set(m, i, (void *)(d + i));
        t = (int *)map_get(m, i);
        assert(t == (d + i));
    }

    for (int i = 0; i < 16; i++) {
        map_set(m, i, (void *)(d + 15 - i));
        t = (int *)map_get(m, i);
        assert(t == (d + 15 - i));
    }

    map_free(&m);
    assert(!m);
}

int main(int argc, const char **argv) {
    clock_t start = clock();

    printf("\n\e[36mRunning tests:\e[0m\n");

    suite(array);
    test(array);

    suite(linked);
    test(linked);

    suite(map);
    test(map);

    printf("\n");
    printf("    \e[90mcompleted in \e[32m%.5fs\e[0m\n", (float) (clock() - start) / CLOCKS_PER_SEC);
    printf("\n");

    return 0;
}
