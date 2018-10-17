#ifndef KIWI_GPU_ACC_MATRIX_NAIVE_HEADER_H_
#define KIWI_GPU_ACC_MATRIX_NAIVE_HEADER_H_

#include <cassert>
#include <future>
#include <vector>

#ifdef _MSVC
#define KIWI_INLINE __forceinline
#else
#define KIWI_INLINE inline
#endif

template <size_t alignment> struct AlignAlloc {
    static void *align_malloc(size_t size) {
        // + alignment make sure there will be an aligned address in the first (alignment)th bytes
        // making sure we are allocating enough.
        void *allocation = std::malloc(size + alignment);

        // alignment is a power of 2 (16, 32, 64)
        //            a  = 0001 0000 = 16
        //        a - 1  = 0000 1111
        //     ~ (a - 1) = 1111 0000
        // b & ~ (a - 1) = Keep the top most ones 0 out the rest (i.e) get the closest power of two
        size_t cp2 = reinterpret_cast<std::size_t>(allocation) & ~std::size_t(alignment - 1);

        // add alignment to it to get a memory address that is inside our allocation & aligned
        void *aligned = reinterpret_cast<void *>(cp2 + alignment);

        // store original pointer before the aligned address for deletion
        *(reinterpret_cast<void **>(aligned) - 1) = allocation;

        return aligned;
    }

    static void free(void *ptr) {
        if(ptr) {
            std::free(*(reinterpret_cast<void **>(ptr) - 1));
        }
    }
};

template <typename T, size_t alignment> class AlignedAllocator {
  public:
    // type definitions
    typedef T value_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    template <typename T> struct rebind { typedef AlignedAllocator<T, alignment> other; };

    pointer address(reference value) const { return &value; }
    const_pointer address(const_reference value) const { return &value; }

    AlignedAllocator() noexcept {}
    AlignedAllocator(const AlignedAllocator &) noexcept {}
    template <typename T, size_t A> AlignedAllocator(const AlignedAllocator<T, A> &) noexcept {}
    ~AlignedAllocator() noexcept {}

    size_type max_size() const noexcept {
        return std::numeric_limits<std::size_t>::max() / sizeof(T);
    }

    pointer allocate(size_type num, const void * = nullptr) {
        return reinterpret_cast<double *>(AlignAlloc<alignment>::align_malloc(num * sizeof(T)));
    }

    void construct(pointer p, const T &value) { new(reinterpret_cast<void *>(p)) T(value); }

    void destroy(pointer p) { p->~T(); }

    void deallocate(pointer p, size_type) {
        AlignAlloc<alignment>::free(reinterpret_cast<void *>(p));
    }
};

template <typename T1, size_t A1, typename T2, size_t A2>
bool operator==(const AlignedAllocator<T1, A1> &, const AlignedAllocator<T2, A2> &) noexcept {
    return A1 == A2;
}
template <typename T1, size_t A1, typename T2, size_t A2>
bool operator!=(const AlignedAllocator<T1, A1> &, const AlignedAllocator<T2, A2> &) noexcept {
    return A1 != A2;
}

template <typename T> struct MemoryMapper2D {
    MemoryMapper2D(size_t r, size_t c, T *ptr) : vrows(r), _rows(r), _cols(c) {
        for(size_t i = 0; i < rows(); ++i) {
            vrows[i] = ptr + i * cols();
        }
    }

    KIWI_INLINE T &operator()(size_t r, size_t c) { return vrows[r][c]; }
    KIWI_INLINE T operator()(size_t r, size_t c) const { return vrows[r][c]; }
    KIWI_INLINE T operator[](size_t t) const { return vrows[0][t]; }

    size_t rows() const { return _rows; }
    size_t cols() const { return _cols; }

    size_t size() const { return rows() * cols(); }

    std::vector<T *> vrows;
    size_t const _rows;
    size_t const _cols;
};

template <typename T> struct MemoryMapper1D {
    MemoryMapper1D(size_t r, size_t c, T *ptr) : ptr(ptr), _rows(r), _cols(c) {}

    KIWI_INLINE T &operator()(size_t r, size_t c) { return ptr[c + r * _cols]; }
    KIWI_INLINE T operator()(size_t r, size_t c) const { return ptr[c + r * _cols]; }
    KIWI_INLINE T operator[](size_t t) const { return ptr[t]; }

    size_t rows() const { return _rows; }
    size_t cols() const { return _cols; }

    size_t size() const { return rows() * cols(); }

    T *ptr;
    size_t const _rows;
    size_t const _cols;
};

using StandardStorage = std::vector<double>;
using SSE128Storage   = std::vector<double, AlignedAllocator<double, 16>>;
using AVX256Storage   = std::vector<double, AlignedAllocator<double, 32>>;
using AVX512Storage   = std::vector<double, AlignedAllocator<double, 64>>;

template <typename MemoryMapper, typename Storage = AVX512Storage> class MatrixNaive {

  public:
    MatrixNaive(size_t r, size_t c, double val = 0) :
        _elems(r * c, val), _mapper(r, c, &_elems[0]) {}

    size_t size() const { return _mapper.size(); }

    KIWI_INLINE double &operator()(size_t r, size_t c) { return _mapper(r, c); }
    KIWI_INLINE double operator()(size_t r, size_t c) const { return _mapper(r, c); }
    KIWI_INLINE double operator[](size_t t) const { return _mapper[t]; }

    size_t rows() const { return _mapper.rows(); }
    size_t cols() const { return _mapper.cols(); }

  private: // 64 for AVX 512 if we ever use it
    Storage _elems;
    MemoryMapper _mapper;
};

template <typename Matrix> Matrix matrix_mult_rows_cols_depth(Matrix const &u, Matrix const &v) {
    assert(u.cols() == v.rows());
    Matrix w(u.rows(), v.cols(), 0);
    for(size_t r = 0; r < w.rows(); r++) {
        for(size_t c = 0; c < w.cols(); c++) {
            for(size_t i = 0; i < u.cols(); ++i) {
                w(r, c) += u(r, i) * v(i, c);
            }
        }
    }
    return w;
}

template <typename Matrix> Matrix matrix_mult_cols_rows_depth(Matrix const &u, Matrix const &v) {
    assert(u.cols() == v.rows());
    Matrix w(u.rows(), v.cols(), 0);

    for(size_t c = 0; c < w.cols(); c++) {
        for(size_t r = 0; r < w.rows(); r++) {
            for(size_t i = 0; i < u.cols(); ++i) {
                w(r, c) += u(r, i) * v(i, c);
            }
        }
    }
    return w;
}

template <typename Matrix> Matrix matrix_mult_depth_cols_rows(Matrix const &u, Matrix const &v) {
    assert(u.cols() == v.rows());
    Matrix w(u.rows(), v.cols(), 0);

    for(size_t i = 0; i < u.cols(); ++i) {
        for(size_t c = 0; c < w.cols(); c++) {
            for(size_t r = 0; r < w.rows(); r++) {
                w(r, c) += u(r, i) * v(i, c);
            }
        }
    }
    return w;
}

template <typename Matrix> Matrix matrix_mult_depth_rows_cols(Matrix const &u, Matrix const &v) {
    assert(u.cols() == v.rows());
    Matrix w(u.rows(), v.cols(), 0);

    for(size_t i = 0; i < u.cols(); ++i) {
        for(size_t r = 0; r < w.rows(); r++) {
            for(size_t c = 0; c < w.cols(); c++) {
                w(r, c) += u(r, i) * v(i, c);
            }
        }
    }
    return w;
}

template <typename Matrix> Matrix matrix_mult_cols_depth_rows(Matrix const &u, Matrix const &v) {
    assert(u.cols() == v.rows());
    Matrix w(u.rows(), v.cols(), 0);

    for(size_t c = 0; c < w.cols(); c++) {
        for(size_t i = 0; i < u.cols(); ++i) {
            for(size_t r = 0; r < w.rows(); r++) {
                w(r, c) += u(r, i) * v(i, c);
            }
        }
    }
    return w;
}

template <typename Matrix> Matrix matrix_mult_rows_depth_cols(Matrix const &u, Matrix const &v) {
    assert(u.cols() == v.rows());
    Matrix w(u.rows(), v.cols(), 0);

    for(size_t r = 0; r < w.rows(); r++) {
        for(size_t i = 0; i < u.cols(); ++i) {
            for(size_t c = 0; c < w.cols(); c++) {
                w(r, c) += u(r, i) * v(i, c);
            }
        }
    }
    return w;
}

template <typename Matrix> Matrix matrix_mult_simd(Matrix const &u, Matrix const &v) {
    assert(u.cols() == v.rows());
    Matrix w(u.rows(), v.cols(), 0);

    for(size_t c = 0; c < w.cols(); c++) {
        for(size_t r = 0; r < w.rows(); r++) {
            for(size_t i = 0; i < u.cols(); ++i) {
                w(r, c) += u(r, i) * v(i, c);
            }
        }
    }
    return w;
}

template <typename Matrix>
Matrix matrix_mult_parallel_rows_cols_depth(Matrix const &u, Matrix const &v) {
    assert(u.cols() == v.rows());
    Matrix w(u.rows(), v.cols(), 0);

    auto task = [&](size_t r) {
        for(size_t c = 0; c < w.cols(); c++) {
            for(size_t i = 0; i < u.cols(); ++i) {
                w(r, c) += u(r, i) * v(i, c);
            }
        }
        return true;
    };

    std::vector<std::future<bool>> tasks(w.rows());
    for(size_t r = 0; r < w.rows(); r++) {
        tasks[r] = std::async(task, r);
    }
    for(auto &future : tasks) {
        future.wait();
    }
    return w;
}

template <typename Matrix>
Matrix matrix_mult_parallel_rows_depth_cols(Matrix const &u, Matrix const &v) {
    assert(u.cols() == v.rows());
    Matrix w(u.rows(), v.cols(), 0);

    auto task = [&](size_t r) {
        for(size_t i = 0; i < u.cols(); ++i) {
            for(size_t c = 0; c < w.cols(); c++) {
                w(r, c) += u(r, i) * v(i, c);
            }
        }
        return true;
    };

    std::vector<std::future<bool>> tasks(w.rows());
    for(size_t r = 0; r < w.rows(); r++) {
        tasks[r] = std::async(task, r);
    }
    for(auto &future : tasks) {
        future.wait();
    }
    return w;
}

template <typename Matrix>
Matrix matrix_mult_parallel_depth_cols_rows(Matrix const &u, Matrix const &v) {
    assert(u.cols() == v.rows());
    Matrix w(u.rows(), v.cols(), 0);

    auto task = [&](size_t i) {
        for(size_t c = 0; c < w.cols(); ++c) {
            for(size_t r = 0; r < w.rows(); r++) {
                w(r, c) += u(r, i) * v(i, c);
            }
        }
        return true;
    };

    std::vector<std::future<bool>> tasks(u.cols());
    for(size_t r = 0; r < u.cols(); r++) {
        tasks[r] = std::async(task, r);
    }
    for(auto &future : tasks) {
        future.wait();
    }
    return w;
}

template <typename Matrix>
Matrix matrix_mult_parallel_depth_rows_cols(Matrix const &u, Matrix const &v) {
    assert(u.cols() == v.rows());
    Matrix w(u.rows(), v.cols(), 0);

    auto task = [&](size_t i) {
        for(size_t r = 0; r < w.rows(); r++) {
            for(size_t c = 0; c < w.cols(); ++c) {
                w(r, c) += u(r, i) * v(i, c);
            }
        }
        return true;
    };

    std::vector<std::future<bool>> tasks(u.cols());
    for(size_t r = 0; r < u.cols(); r++) {
        tasks[r] = std::async(task, r);
    }
    for(auto &future : tasks) {
        future.wait();
    }
    return w;
}

template <typename Matrix>
Matrix matrix_mult_parallel_cols_depth_rows(Matrix const &u, Matrix const &v) {
    assert(u.cols() == v.rows());
    Matrix w(u.rows(), v.cols(), 0);

    auto task = [&](size_t c) {
        for(size_t i = 0; i < u.cols(); i++) {
            for(size_t r = 0; r < w.rows(); ++r) {
                w(r, c) += u(r, i) * v(i, c);
            }
        }
        return true;
    };

    std::vector<std::future<bool>> tasks(w.cols());
    for(size_t r = 0; r < w.cols(); r++) {
        tasks[r] = std::async(task, r);
    }
    for(auto &future : tasks) {
        future.wait();
    }
    return w;
}

template <typename Matrix>
Matrix matrix_mult_parallel_cols_rows_depth(Matrix const &u, Matrix const &v) {
    assert(u.cols() == v.rows());
    Matrix w(u.rows(), v.cols(), 0);

    auto task = [&](size_t c) {
        for(size_t r = 0; r < w.rows(); ++r) {
            for(size_t i = 0; i < u.cols(); i++) {
                w(r, c) += u(r, i) * v(i, c);
            }
        }
        return true;
    };

    std::vector<std::future<bool>> tasks(w.cols());
    for(size_t r = 0; r < w.cols(); r++) {
        tasks[r] = std::async(task, r);
    }
    for(auto &future : tasks) {
        future.wait();
    }
    return w;
}

template <typename Matrix> double sum(Matrix const &u) {
    double s = 0;
    for(size_t i = 0; i < u.size(); ++i)
        s += u[i];
    return s;
}

#endif
