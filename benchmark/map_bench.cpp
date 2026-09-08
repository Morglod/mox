//   robin_hood::unordered_flat_map - official robin-hood (vs mox dense HashMap)
//   ankerl::unordered_dense::map   - official dense design mox HashMap follows
//   phmap::flat_hash_map           - swiss table (header-only absl port; vs mox SwissMap)
//   std::unordered_map             - node-based baseline (vs mox NodeMap)
// clang++ -O3 -march=native -std=c++20 ./map_bench.cpp

#include <cstdint>
#include <cstdio>
#include <unordered_map>
#ifdef _WIN32
#include <windows.h>
#else
#include <ctime>
#endif

#include "robin_hood/robin_hood.h"
#include "unordered_dense/unordered_dense.h"
#include "parallel_hashmap/phmap.h"

static constexpr int64_t N = 1000000;

static uint64_t mix64(uint64_t x) {
    x ^= x >> 30;
    x *= 0xBF58476D1CE4E5B9ull;
    x ^= x >> 27;
    x *= 0x94D049BB133111EBull;
    return x ^ (x >> 31);
}

static int64_t key_at(int64_t i) {
    return (int64_t)(mix64((uint64_t)i + 0x9E3779B97F4A7C15ull) & 0x7FFFFFFFFFFFFFFFull);
}

struct Mix64Hash {
    using is_avalanching = void; // tells unordered_dense not to re-mix
    size_t operator()(int64_t v) const noexcept { return (size_t)mix64((uint64_t)v); }
};

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

static void report(const char *name, const char *phase, int64_t t0, int64_t ops, int64_t sink) {
    int64_t dt = now_ns() - t0;
    printf("%s %s: %lld ns/op (%lld ms total, sink %lld)\n", name, phase,
           (long long)(dt / ops), (long long)(dt / 1000000), (long long)(sink & 1));
}

template <class Map>
static void bench(const char *name) {
    Map m;
    int64_t t = now_ns();
    for (int64_t i = 0; i < N; i++) m[key_at(i)] = i;
    report(name, "insert", t, N, 0);

    int64_t sink = 0;
    t = now_ns();
    for (int64_t i = 0; i < N; i++) sink += m.find(key_at(i))->second;
    report(name, "find-hit", t, N, sink);

    int64_t misses = 0;
    t = now_ns();
    for (int64_t i = 0; i < N; i++) {
        if (m.find(key_at(i + N)) != m.end()) misses += 1;
    }
    report(name, "find-miss", t, N, misses);

    t = now_ns();
    for (int64_t i = 0; i < N; i += 2) m.erase(key_at(i));
    report(name, "erase-half", t, N / 2, 0);

    sink = 0;
    t = now_ns();
    for (int64_t i = 0; i < N; i++) {
        auto it = m.find(key_at(i));
        if (it != m.end()) sink += it->second;
    }
    report(name, "refind", t, N, sink);
}

int main(void) {
    bench<robin_hood::unordered_flat_map<int64_t, int64_t, Mix64Hash>>("rh_flat ");
    bench<ankerl::unordered_dense::map<int64_t, int64_t, Mix64Hash>>("uo_dense");
    bench<phmap::flat_hash_map<int64_t, int64_t, Mix64Hash>>("phmap   ");
    bench<std::unordered_map<int64_t, int64_t, Mix64Hash>>("std_umap");
    return 0;
}
