#include <stdlib.h>

#include <compiler.h>
#include <darray.h>


#define BITS (8*sizeof(dkey_t))

typedef struct darray darray_t;

struct darray {
    dkey_t base;
    union {
        darray_t *sub;
        darray_val_t val;
    };
};

unsigned mem=0;

inline static dkey_t level_diff(dkey_t k) {
    dkey_t n=0;
    while (k) {
        n += 4;
        k >>= 4;
    }

    return n;
}


static inline dkey_t get_level(const darray_t *n) {
    return n->base >> (BITS-9);
}

/*
static inline dkey_t get_index(const darray_t *n, dkey_t i) {
    return popcount16(n->base << (16-i)) & -((n->base >> i) & 1);
}
*/


static inline dkey_t get_index(const darray_t *n, dkey_t i) {
    i = n->base << (15-i);
    return popcount16(i) - ((i & 0xffff) != 0);
}

static inline dkey_t get_key(const darray_t *n) {
    return ~n->base;
}

static inline void set_key(darray_t *n, dkey_t key) {
    n->base = ~key;
}

static inline darray_t *get_base(const darray_t *n, const dkey_t key) {
    dkey_t l;

    while ((n->base >> (BITS-1)) == 0) {
        l = get_level(n);
        n = n->sub + get_index(n, (key >> l) & 0xf);
    }
    return (darray_t *)n;
}

static dkey_t get_path(const darray_t *n, const dkey_t key, darray_t *p) {
    dkey_t l, i=0;

    while ((n->base >> (BITS-1)) == 0) {
        l = get_level(n);
        p[i].sub = (darray_t *)n;
        p[i++].base = l;
        n = n->sub + get_index(n, (key >> l) & 0xf);
    }
    p[i].sub = (darray_t *)n;
    p[i++].base = get_level(n);
    return i;
}


darray_val_t *darray_find(void **da, dkey_t key) {
    darray_t *n = *da;

    if (n == NULL) {
        return NULL;
    }

    n = get_base(n, key);
    if (get_key(n) == key) {
        return &(n->val);
    }
    return NULL;
}


int darray_get(void **da, dkey_t key, darray_val_t **val) {
    darray_t *n = *da, *tmp;
    dkey_t k, l, m, i;
    darray_t p[BITS/4];

    if (n != NULL) {
        i = get_path(n, key, p)-1;
        k = get_key(p[i].sub);
        l = k ^ key;

        if (l == 0) {
            *val = &(p[i].sub->val);
            return 1;
        }

        m = level_diff(l) - 4;
        tmp = p;

        while (tmp->base > m && tmp->base < 256) {
            tmp++;
        }

        l = tmp->base;
        n = tmp->sub;

        if (l==m) {
            l = (n->base >> 16) & 0xff; //popcount16(n->base);
            if ((l & 1) == 0) {
                mem += 2*sizeof(darray_t);
                tmp = realloc(n->sub, (l+2)*sizeof(darray_t));
                if (tmp == NULL) {
                    return -1;
                }
                n->sub = tmp;
            }
            k = (key >> m) & 0xf;
            n->base |= (1 << k);
            n->base += (1<<16);
            k = get_index(n, k);

            tmp = n->sub + k;
            l -= k;
            while (l) {
                tmp[l] = tmp[l-1];
                l--;
            }

            goto set_key;
        } else {
            mem += 2*sizeof(darray_t);
            tmp = (darray_t *)malloc(2*sizeof(darray_t));
            if (tmp == NULL) {
                return -1;
            }
            tmp[0] = tmp[1] = *n;
            n->sub = tmp;

            l = (key >> m) & 0xf;
            k = (k >> m) & 0xf;

            n->base = (1 << l) | (1 << k) | (2<<16) | (m << (BITS-9));
            tmp += (k < l);

            set_key:
            set_key(tmp, key);
            *val = &(tmp->val);
            return 0;
        }
    }

    mem += sizeof(darray_t);
    n = (darray_t *)malloc(sizeof(darray_t));
    if (n == NULL) {
        return -1;
    }
    set_key(n, key);
    *da = n;
    *val = &(n->val);
    return 0;
}

int darray_remove(void **da, dkey_t key, darray_val_t *val) {
    darray_t *n = *da, *tmp;
    dkey_t k, l, i;
    darray_t p[BITS/4];

    if (n != NULL) {
        i = get_path(n, key, p);
        i--;
        k = get_key(p[i].sub);

        if (k == key) {
            *val = p[i].sub->val;
            if (i==0) {
                *da = NULL;
                free(p[i].sub);
                return 1;
            }


            n = p[i-1].sub;
            tmp = n->sub;
            l = (n->base >> 16) & 0xff; //popcount16(n->base);
            k = p[i].sub - tmp;

            if (l == 2) {
                *n = tmp[k ^ 1];
                free(tmp);
                return 1;
            }

            l--;
            while (k<l) {
                tmp[k] = tmp[k+1];
                k++;
            }

            if ((l & 1) == 0) {
                n->sub = realloc(tmp, l*sizeof(darray_t));
            }
            n->base -= (1<<16);
            n->base ^= (1 << ((key >> p[i-1].base) & 0xf));
            return 1;
        }
    }
    return 0;
}

static darray_val_t *_min(darray_t *n, dkey_t *min) {

    while (get_level(n) < 256) {
        n = n->sub;
    }
    *min = get_key(n);
    return &n->val;
}

darray_val_t *darray_min(void **da, dkey_t *min) {
    if (*da == NULL) {
        return NULL;
    }

    return _min(*da, min);
}

static inline darray_val_t *_max(darray_t *n, dkey_t *max) {

    while (get_level(n) < 256) {
        n = n->sub + popcount16(n->base) - 1;
    }
    *max = get_key(n);
    return &n->val;
}

darray_val_t *darray_max(void **da, dkey_t *max) {
    if (*da == NULL) {
        return NULL;
    }

    return _max(*da, max);
}

darray_val_t *darray_prev(void **da, dkey_t key, dkey_t *prev) {
    darray_t *n = *da;
    dkey_t k, l, m, i;
    darray_t p[BITS/4];

    if (n != NULL) {
        i = get_path(n, key, p)-1;
        k = get_key(p[i].sub);
        l = k ^ key;

        if (l == 0) {
            *prev = k;
            return &(p[i].sub->val);
        }

        m = level_diff(l) - 4;

        n = p;


        while (n->base >= m && n->base < 256) {
            n++;
        }

        if (k < key) {
            return _max(n->sub, prev);
        }

        while (n != p) {
            n--;
            l = (n->sub->base << (16 - ((key >> n->base) & 0xf))) & 0xffff;
            if (l) {
                return _max(n->sub->sub + popcount16(l) - 1, prev);
            }
        }
    }
    return NULL;
}

darray_val_t *darray_next(void **da, dkey_t key, dkey_t *next) {
    darray_t *n = *da;
    dkey_t k, l, m, i;
    darray_t p[BITS/4];

    if (n != NULL) {
        i = get_path(n, key, p)-1;
        k = get_key(p[i].sub);
        l = k ^ key;

        if (l == 0) {
            *next = k;
            return &(p[i].sub->val);
        }

        m = level_diff(l) - 4;

        n = p;


        while (n->base >= m && n->base < 256) {
            n++;
        }

        if (k > key) {
            return _min(n->sub, next);
        }

        while (n != p) {
            n--;
            l = (n->sub->base & 0xffff) << (15 - ((key >> n->base) & 0xf));
            if (l >> 16) {
                return _min(n->sub->sub + popcount16(l), next);
            }
        }
    }
    return NULL;
}
