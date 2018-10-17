#ifndef KIWI_GPU_ACC_MATRIX_EXPR_HEADER_H_
#define KIWI_GPU_ACC_MATRIX_EXPR_HEADER_H_

#include <cassert>
#include <vector>

namespace cpu {

struct ExecTrait {
    int thread_count         = 1;
    bool parallelism_enabled = false;
};

template <typename E, typename ExecT> class MatrixExpression {
  public:
    double cpu_eval(size_t r, size_t c) const { return static_cast<E const &>(*this)(r, c); }
    double gpu_eval(size_t r, size_t c) const { return static_cast<E const &>(*this)(r, c); }

    size_t size() const { return rows() * cols(); }
    size_t rows() const { return static_cast<E const &>(*this).rows(); }
    size_t cols() const { return static_cast<E const &>(*this).cols(); }
};

template <typename E1, typename E2, typename ExecT>
class MatrixSum : public MatrixExpression<MatrixSum<E1, E2, ExecT>, ExecT> {
    E1 const &_u;
    E2 const &_v;

  public:
    MatrixSum(E1 const &u, E2 const &v) : _u(u), _v(v) { assert(u.size() == v.size()); }

    double cpu_eval(size_t r, size_t c) const { return _u(r, c) + _v(r, c); }
    double gpu_eval(size_t r, size_t c) const { return _u(r, c) + _v(r, c); }

    size_t rows() const { return _v.rows(); }
    size_t cols() const { return _v.cols(); }
};

template <typename E1, typename E2, typename ExecT>
class MatrixMult : public MatrixExpression<MatrixMult<E1, E2, ExecT>, ExecT> {
    E1 const &_u;
    E2 const &_v;

  public:
    MatrixMult(E1 const &u, E2 const &v) : _u(u), _v(v) { assert(u.cols() == v.rows()); }

    double cpu_eval(size_t r, size_t c) const {
        double sum = 0;
        for(size_t i = 0; i < _u.cols(); ++i)
            sum += _u(r, i) * _v(i, c);
        return sum;
    }

    double gpu_eval(size_t r, size_t c) const {}

    /*
    __global__
    void kernel(int n, float* x, float y){
        for (int i = 0; i < n; i++)
              y[i] = x[i] + y[i];
    }*/

    size_t rows() const { return _u.rows(); }
    size_t cols() const { return _v.cols(); }
};

template <typename E1, typename E2, typename ExecT>
MatrixSum<E1, E2, ExecT> operator+(E1 const &u, E2 const &v) {
    return MatrixSum<E1, E2, ExecT>(u, v);
}

template <typename E1, typename E2, typename ExecT>
MatrixMult<E1, E2, ExecT> operator*(E1 const &u, E2 const &v) {
    return MatrixMult<E1, E2, ExecT>(u, v);
}

template <typename ExecT> class MatrixExpr : public MatrixExpression<MatrixExpr<ExecT>, ExecT> {
    std::vector<double> elems;
    std::vector<double *> vrows;
    size_t _rows;
    size_t _cols;

  public:
    MatrixExpr(size_t r, size_t c) : elems(r * c), vrows(r), _rows(r), _cols(c) {
        for(size_t i = 0; i < rows(); ++i) {
            vrows[i] = &elems[i * cols()];
        }
    }

    double &operator()(size_t r, size_t c) { return vrows[r][c]; }
    double operator()(size_t r, size_t c) const { return vrows[r][c]; }

    size_t rows() const { return _rows; }
    size_t cols() const { return _cols; }

    // construct vector using initializer list
    MatrixExpr(std::initializer_list<double> init) {
        for(auto i : init)
            elems.push_back(i);
    }

    template <typename E> MatrixExpr(MatrixExpression<E, ExecT> const &vec) : elems(vec.size()) {
        {
            size_t r = 0;
            size_t c = 0;
            // clang-format off
            omp_set_num_threads(ExecT::thread_count);
            #pragma omp parallel for private(r, c) if(ExecT::parallelism_enabled) num_threads(ExecT::thread_count)
            for(; r < vec.rows(); ++r) {
                for(; c < vec.cols(); ++c) {
                    (*this)(r, c) = vec(r, c);
                }
            }
            // clang-format on
        }
    }
};
} // namespace cpu

#endif
