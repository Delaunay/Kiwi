#include <emmintrin.h>
#include <immintrin.h>
#include <xmmintrin.h>

#include <chrono>
#include <cmath>
#include <cstdio>
#include <functional>

#if defined(__GNUC__) && !defined(__AVX__) /* sanity check */
#error "compiling simd-avx.h without -mavx"
#endif

struct GenericVectorAdd {
    static __forceinline void add(float *a, float *b, float *c, size_t n) {
        for(size_t i = 0; i < n; ++i)
            c[i] = a[i] + b[i];
    }

    static __forceinline void add(float *a, float *b, float *c) { add(a, b, c, 4); }
};

struct VectorAdd4 {
    static __forceinline void add(float *a, float *b, float *c) {
        c[0] = a[0] + b[0];
        c[1] = a[1] + b[1];
        c[2] = a[2] + b[2];
        c[3] = a[3] + b[3];
    }
};

// Block of 4 floats or 2 doubles
struct SSEAdd {
    /* sse(float*, float*, float*):
     *   vmovaps xmm0, XMMWORD PTR [rdi]
     *   vaddps  xmm0, xmm0, DWORD PTR [rsi]
     *   vmovaps XMMWORD PTR [rdx], xmm0
     *   ret
     */
    static __forceinline void add(float *a, float *b, float *c) {
        __m128 ptr_a = _mm_load_ps(a);
        __m128 ptr_b = _mm_load_ps(b);
        _mm_store_ps(c, _mm_add_ps(ptr_a, ptr_b));
    }

    static __forceinline void add(double *a, double *b, double *c) {
        __m128d ptr_a = _mm_load_pd(a);
        __m128d ptr_b = _mm_load_pd(b);
        _mm_store_pd(c, _mm_add_pd(ptr_a, ptr_b));
    }
};

// Block of 8 floats or 4 doubles
struct AVXAdd {
    /* avx_add(float*, float*, float*):
     *   vmovaps ymm0, YMMWORD PTR [rdi]
     *   vaddps  ymm0, ymm0, YMMWORD PTR [rsi]
     *   vmovaps YMMWORD PTR [rdx], ymm0
     *   vzeroupper
     *   ret
     */
    static __forceinline void add(float *a, float *b, float *c) {
        _mm256_zeroupper();
        __m256 ptr_a = _mm256_load_ps(a);
        __m256 ptr_b = _mm256_load_ps(b);
        _mm256_store_ps(c, _mm256_add_ps(ptr_a, ptr_b));
        _mm256_zeroupper();
    }

    static __forceinline void add(double *a, double *b, double *c) {
        _mm256_zeroupper();
        __m256d ptr_a = _mm256_load_pd(a);
        __m256d ptr_b = _mm256_load_pd(b);
        _mm256_store_pd(c, _mm256_add_pd(ptr_a, ptr_b));
        _mm256_zeroupper();
    }
};

template <typename T, int batch_size = 4>
__forceinline void batched_add(float *a, float *b, float *c, size_t n) {
    size_t i = 0;
    for(i = 0; i < n; i += batch_size) {
        T::add(a + i, b + i, c + i);
    }
}

struct Vec4 {
    float x;
    float y;
    float z;
    float a;
};

class TimeIt {
  public:
    using TimePoint = std::chrono::high_resolution_clock::time_point;

    TimePoint start = std::chrono::high_resolution_clock::now();

    double stop() {
        TimePoint end = std::chrono::high_resolution_clock::now();
        double seconds =
            std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
        return seconds;
    }
};

class StatStream {
  public:
    StatStream &operator+=(double val) {
        sum += val;
        sum_squared += val * val;
        count += 1;
        return *this;
    }

    double mean() { return sum / double(count); }

    double var() { return sum_squared / double(count) - mean() * mean(); }

    double sd() { return sqrt(var()); }

    double sum         = 0;
    double sum_squared = 0;
    std::size_t count  = 0;
};

template <typename T> void benchmark(size_t test_rep, size_t test_count, const char *msg, T fun) {
    StatStream stats;
    // using ReturnType = typename std::result_of<T>::type;
    fun();

    for(size_t i = 0; i < test_count; ++i) {
        TimeIt chrono;
        for(size_t j = 0; j < test_rep; ++j) {
            fun();
        }
        stats += chrono.stop();
    }
    printf("%-30s: (avg=%8.4f, sd=%8.4f, n=%llu)\n", msg, stats.mean(), stats.sd(), stats.count);
    fflush(stdout);
};

int main() {
    size_t mio        = 1024 * 1024;
    size_t total_size = 8 * mio;
    size_t size       = total_size / sizeof(float);

    size_t remaining = sizeof(float) * size % 32;
    size_t alignment = remaining == 0 ? 0 : 32 - remaining;

    printf("Alignment %llu\n", alignment);

    //__attribute__((aligned(32)))
    float *va = new float[size];
    float *vb = new float[size];
    float *vc = new float[size];

    //*
    {
        size_t test_rep   = size * 100;
        size_t test_count = 30;

        printf("# Vector4\n");
        printf("-----------\n");
        printf("Test Repetition: %llu \n", test_rep);
        printf("Test Count     : %llu \n\n", test_count);

        benchmark(test_rep, test_count, "Manual Unrolled", [&]() {
            VectorAdd4::add(va, vb, vc);
            return 1;
        });

        benchmark(test_rep, test_count, "Generic", [&]() {
            GenericVectorAdd::add(va, vb, vc, 4);
            return 1;
        });

        benchmark(test_rep, test_count, "SSE", [&]() {
            SSEAdd::add(va, vb, vc);
            return 1;
        });
    }
    printf("\n\n"); //*/

    {
        size_t test_rep   = 100;
        size_t test_count = 30;

        printf("# Vector %llu \n", size);
        printf("----------------\n");
        printf("Test Repetition: %llu \n", test_rep);
        printf("Test Count     : %llu \n\n", test_count);

        //*
        benchmark(test_rep, test_count, "Batch Manual Unrolled", [&]() {
            batched_add<VectorAdd4>(va, vb, vc, size);
            return 1;
        });
        benchmark(test_rep, test_count, "Batch generic", [&]() {
            batched_add<GenericVectorAdd>(va, vb, vc, size);
            return 1;
        });
        benchmark(test_rep, test_count, "Generic", [&]() {
            GenericVectorAdd::add(va, vb, vc, size);
            return 1;
        });
        benchmark(test_rep, test_count, "Batch SSE", [&]() {
            batched_add<SSEAdd>(va, vb, vc, size);
            return 1;
        }); //*/
        benchmark(test_rep, test_count, "Batch AVX", [&]() {
            batched_add<AVXAdd, 8>(va, vb, vc, size);
            return 1;
        });
    }
    delete[] va;
    delete[] vb;
    delete[] vc;
    return 0;
};
