#ifndef KIWI_GPU_ACC_VECTOR_EXPR_MP_HEADER_H_
#define KIWI_GPU_ACC_VECTOR_EXPR_MP_HEADER_H_

#include "VectorExpression.h"
#include <omp.h>

namespace cpu {

template <int ThreadNum> class VecExprMp : public VecExpression<VecExpr> {
    std::vector<double> elems;

  public:
    enum { thread_count = ThreadNum, parallelism_enabled = true };

    double operator[](size_t i) const { return elems[i]; }
    double &operator[](size_t i) { return elems[i]; }
    size_t size() const { return elems.size(); }

    VecExprMp(size_t n) : elems(n) {}

    // construct vector using initializer list
    VecExprMp(std::initializer_list<double> init) {
        for(auto i : init)
            elems.push_back(i);
    }

    // A Vec can be constructed from any VecExpression, forcing its evaluation.
    template <typename E> VecExprMp(VecExpression<E> const &vec) : elems(vec.size()) {
        // clang-format off
        {
            int i;
            #pragma omp parallel for private(i) if(parallelism_enabled) num_threads(thread_count)
            for(i = 0; i < vec.size(); ++i) {
                elems[i] = vec[i];
            }
        }
        // clang-format on
    }
};
} // namespace cpu

#endif
