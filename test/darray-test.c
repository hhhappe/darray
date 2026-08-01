#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <timer.h>


#include <darray.h>
extern int mem;

static void *da = NULL;
static int err;

static inline void set_index(uintptr_t key, uintptr_t v) {
    darray_val_t *val;
    err = darray_get(&da, key, &val);
    val->val = v;
}

static inline uintptr_t get_index(uintptr_t key) {
    return darray_find(&da, key)->val;
}

static inline uintptr_t get_prev(uintptr_t key) {
    uintptr_t r;
    darray_prev(&da, key, &r);
    return r;
}

static inline uintptr_t get_next(uintptr_t key) {
    uintptr_t r;
    darray_next(&da, key, &r);
    return r;
}

static inline void remove_index(uintptr_t key) {
    darray_val_t out;
    darray_remove(&da, key, &out);
}

static inline uintptr_t mem_size() {
    return mem;
}

int main(int argc, char **args) {
    double t;
    uintptr_t first, its, delta, max;
    uintptr_t i, j, k;
    first = atoll(args[1]);
    its = atoll(args[2]);
    delta = atoll(args[3]);
    max = first + its*delta;
    srandom(666);

    printf("first=%" PDKEY "\n", first);
    printf("its=%" PDKEY "\n", its);
    printf("delta=%" PDKEY "\n", delta);

//    val = darray_get(&da, 0);
//    val = darray_get(&da, 1);
//    val = darray_get(&da, 2);
    t = nsec();
    t = nsec();
    for (i=first; i<max; i+=delta) {
        set_index(i, i);
    }
    t = nsec()-t;
    printf("set time=%f mem=%u\n", (t/its), mem);
//    srandom(666);

//    get_index(16);
    t = nsec();

    for (j=0; j<10; j++) {
//    srandom(666);
        for (i=first; i<max; i+=delta) {
            k = get_index(i);
            if (k != i) {
                printf("err: i=%" PDKEY "\n", i);
            }
        }
    }
    t = nsec()-t;

    printf("get time=%f\n", (t/(10*its)));
    t = nsec();
    for (i=first; i<max; i+=delta) {
        k = get_prev(i+delta/2);

/*
        if (j != r) {
            printf("err: r=%" PDKEY " j=%" PDKEY "\n", r, j);
        }
*/
    }
    t = nsec()-t;

    printf("prev time=%f\n", (t/its));
    j = 666;

    t = nsec();
    for (i=first+delta; i<max; i+=delta) {
        k = get_next(i-delta/2);
        if (k != i) {
            printf("err: r=%" PDKEY " j=%" PDKEY "\n", i, k);
        }
    }
    t = nsec()-t;

    printf("next time=%f\n", (t/its));

    t = nsec();
    for (i=first; i<max; i+=delta) {
        remove_index(i);
    }
    t = nsec()-t;

    printf("remove time=%f\n", (t/its));
    return 0;
}
