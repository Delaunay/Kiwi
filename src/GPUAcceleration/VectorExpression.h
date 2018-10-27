#ifndef KIWI_GPU_ACC_VECTOR_EXPR_HEADER_H_
#define KIWI_GPU_ACC_VECTOR_EXPR_HEADER_H_

#include <cassert>
#include <vector>

namespace cpu {

template <typename E> class VecExpression {
  public:
    double operator[](size_t i) const { return static_cast<E const &>(*this)[i]; }

    size_t size() const { return static_cast<E const &>(*this).size(); }

    // reducer eval
    template <typename Reducer> double reduce(Reducer reducer, double init = 0) {
        // clang-format off
        double sum = init;
        {
            int i;
            #pragma omp parallel for private(i) if(E::parallelism_enabled) num_threads(E::thread_count)
            for(i = 0; i < size(); ++i) {
                sum = reducer(sum, (*this)[i]);
            }
        }
        return sum;
        // clang-format on
    }
};

template <typename E1, typename E2> class VecSum : public VecExpression<VecSum<E1, E2>> {
    E1 const &_u;
    E2 const &_v;

  public:
    enum { thread_count = E1::thread_count, parallelism_enabled = E1::parallelism_enabled };

    VecSum(E1 const &u, E2 const &v) : _u(u), _v(v) { assert(u.size() == v.size()); }

    double operator[](size_t i) const { return _u[i] + _v[i]; }

    size_t size() const { return _v.size(); }
};

template <typename E1, typename E2> class VecMult : public VecExpression<VecMult<E1, E2>> {
    E1 const &_u;
    E2 const &_v;

  public:
    enum { thread_count = E1::thread_count, parallelism_enabled = E1::parallelism_enabled };

    VecMult(E1 const &u, E2 const &v) : _u(u), _v(v) { assert(u.size() == v.size()); }

    double operator[](size_t i) const { return _u[i] * _v[i]; }

    size_t size() const { return _v.size(); }
};

template <typename E1, typename E2> VecSum<E1, E2> operator+(E1 const &u, E2 const &v) {
    return VecSum<E1, E2>(u, v);
}

template <typename E1, typename E2> VecMult<E1, E2> operator*(E1 const &u, E2 const &v) {
    return VecMult<E1, E2>(u, v);
}

class VecExpr : public VecExpression<VecExpr> {
    std::vector<double> elems;

  public:
    enum { thread_count = 1, parallelism_enabled = false };

    double operator[](size_t i) const { return elems[i]; }
    double &operator[](size_t i) { return elems[i]; }
    size_t size() const { return elems.size(); }

    VecExpr(size_t n) : elems(n) {}

    // construct vector using initializer list
    VecExpr(std::initializer_list<double> init) {
        for(auto i : init)
            elems.push_back(i);
    }

    // A Vec can be constructed from any VecExpression, forcing its evaluation.
    template <typename E> VecExpr(VecExpression<E> const &vec) : elems(vec.size()) {
        for(size_t i = 0; i != vec.size(); ++i) {
            elems[i] = vec[i];
        }
    }
};

template <typename E1, typename E2> double dot(E1 const &u, E2 const &v) {
    return VecMult<E1, E2>(u, v).reduce([](double a, double b) { return a + b; }, 0);
}

} // namespace cpu

#endif
