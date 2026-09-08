// clang -O3 -march=native ./rapidhash_bench.c

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

#include "rapidhash/rapidhash_v3.h"

#define BUF_SIZE 16384

static uint64_t sm64(uint64_t x) {
    x ^= x >> 30;
    x *= 0xBF58476D1CE4E5B9ull;
    x ^= x >> 27;
    x *= 0x94D049BB133111EBull;
    return x ^ (x >> 31);
}

static int64_t iters_for(int64_t len) {
    int64_t n = 400000000 / (len + 32);
    if (n > 4000000) n = 4000000;
    if (n < 20000) n = 20000;
    return n;
}

static int64_t now_ns(void) {
#ifdef _WIN32
    LARGE_INTEGER f, c;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c);
    return (int64_t)((double)c.QuadPart * 1e9 / (double)f.QuadPart);
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)ts.tv_sec * 1000000000ll + ts.tv_nsec;
#endif
}

#define BENCH_LEN(name, F, buf, len)                                             \
    do {                                                                         \
        const int64_t bl_len = (len);                                            \
        const int64_t bl_iters = iters_for(bl_len);                              \
        uint64_t bl_h = 0;                                                       \
        for (int64_t bl_w = 0; bl_w < bl_iters / 10 + 1; bl_w++)                 \
            bl_h = F(buf, (size_t)bl_len, bl_h);                                 \
        bl_h = 0;                                                                \
        int64_t bl_t = now_ns();                                                 \
        for (int64_t bl_i = 0; bl_i < bl_iters; bl_i++)                          \
            bl_h = F(buf, (size_t)bl_len, bl_h);                                 \
        int64_t bl_dt = now_ns() - bl_t;                                         \
        if (bl_dt < 1) bl_dt = 1;                                                \
        printf("%s len=%lld: %.2f ns/hash, %.2f MB/s, sink=%llu\n", name,        \
               (long long)bl_len, (double)bl_dt / (double)bl_iters,              \
               (double)(bl_len * bl_iters) * 1000.0 / (double)bl_dt,             \
               (unsigned long long)bl_h);                                        \
    } while (0)

int main(void) {
    uint8_t *buf = (uint8_t *)malloc(BUF_SIZE);
    for (int64_t i = 0; i < BUF_SIZE; i++) {
        buf[i] = (uint8_t)sm64((uint64_t)i * 0x9E3779B97F4A7C15ull + 0x123456789ABCDEF0ull);
    }

    static const int64_t sizes[15] = {2, 4, 8, 12, 16, 24, 32, 48, 64, 112, 256, 512, 1024, 4096, 16384};

    for (int i = 0; i < 15; i++) BENCH_LEN("c   rapidhash     ", rapidhash_withSeed, buf, sizes[i]);
    for (int i = 0; i < 15; i++) BENCH_LEN("c   rapidhashMicro", rapidhashMicro_withSeed, buf, sizes[i]);
    for (int i = 0; i < 15; i++) BENCH_LEN("c   rapidhashNano ", rapidhashNano_withSeed, buf, sizes[i]);

    free(buf);
    return 0;
}
