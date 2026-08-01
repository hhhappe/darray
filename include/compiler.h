#ifndef _COMPILER_H
#define _COMPILER_H

#include <inttypes.h>

#define noinline __attribute__((noinline))
#define attr_packed __attribute__((__packed__))
#define bexpect(expr, val) __builtin_expect((expr), (val))
#define iffp(expr) if(__builtin_expect((expr), 1))
#define ifsp(expr) if(__builtin_expect((expr), 0))
#define whilefp(expr) while(__builtin_expect((expr), 1))
#define whilesp(expr) while(__builtin_expect((expr), 1))

#define popcount(val) __builtin_popcount(val)
#define popcountl(val) __builtin_popcountl(val)
#define ctz(val) __builtin_ctz (val)

inline static int clzl(unsigned long val) {
    int n=8*sizeof(val);
    while (val) {
        n--;
        val >>= 1;
    }

    return n;
}

/*
inline static int clzl(unsigned long val) {
    int n=0, m = (0xf << (8*sizeof(val) - 4));

    while ((val & m) == 0) {
       n += 4;
       m >>= 4;
    }
    return n;
}
*/
/*
inline static int clzl(unsigned long val) {
    unsigned long n;
    unsigned long m = -1; //(0xff << (8*sizeof(val) - 8));

    n = ((val & m) == 0);
    m <<= 8;
    n += ((val & m) == 0);
    m <<= 8;
    n += ((val & m) == 0);
    m <<= 8;
    n += ((val & m) == 0);
    n *= 8;
    m <<= 4;
    n += 4*((val & (m >> n)) == 0);

    return n;
}
*/
/*
inline static int clzl(unsigned long val) {
    unsigned long n;
    unsigned long m = -1; //(0xff << (8*sizeof(val) - 8));

    n = ((val & m) == 0);
    m <<= 8;
    n += ((val & m) == 0);
    m <<= 8;
    n += ((val & m) == 0);
    m <<= 8;
    n += ((val & m) == 0);
    n *= 8;
    m <<= 4;
    n += 4*((val & (m >> n)) == 0);

    return n;
}
*/
/*
inline static int clzl(unsigned long val) {
    unsigned long n;
    unsigned long m = (-1) << 16; //(0xff << (8*sizeof(val) - 8));

    n = 16*((val & m) != 0);
    m = (-1) << (n + 8);
    n += 8*((val & m) != 0);
    m = (-1) << (n + 4);
    n += 4*((val & m) != 0);

    return 28-n;
}
*/
/*
static uint64_t map = 0x4332322132212110ll;

inline static unsigned long popcount16(unsigned long r) {
//    uint64_t map = 0x8da691691448ll;
    //1000 1101 1010 0110 1001 0001 0110 1001 0001 0100 0100 1000;

                    //{0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
    uint64_t n;

//    printf("%x\n", r);

    r <<= 2;

    n = (map >> (r & 0x3c)) & 0xf;
    r >>= 4;
    n += (map >> (r & 0x3c)) & 0xf;
    r >>= 4;
    n += (map >> (r & 0x3c)) & 0xf;
    r >>= 4;
    n += (map >> (r & 0x3c)) & 0xf;

//    printf("%llx\n", n);

    return n;
}
*/

inline static unsigned long popcount16(unsigned long r) {
    r &= 0xffff;
    r += (r & 0x7777);
    r += (r & 0x6666);
    r += (r & 0x4444);
    r = ((r >> 3) + (r >> 7)) & 0x0f0f;

    return ((r >> 8) + r) & 0xff;
}


#endif
