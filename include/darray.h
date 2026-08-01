#ifndef _DARRAY_H
#define _DARRAY_H

#include <inttypes.h>
#define PDKEY PRIuPTR

typedef uintptr_t dkey_t;

#ifdef __cplusplus
  extern "C" {
#endif

typedef union {
    dkey_t val;
    void *ptr;
} darray_val_t;


darray_val_t *darray_find(void **da, dkey_t key);
int darray_get(void **da, dkey_t key, darray_val_t **val);
int darray_remove(void **da, dkey_t key, darray_val_t *val);
darray_val_t *darray_min(void **da, dkey_t *min);
darray_val_t *darray_max(void **da, dkey_t *max);
darray_val_t *darray_prev(void **da, dkey_t key, dkey_t *prev);
darray_val_t *darray_next(void **da, dkey_t key, dkey_t *next);

#ifdef __cplusplus
  }
#endif
#endif
