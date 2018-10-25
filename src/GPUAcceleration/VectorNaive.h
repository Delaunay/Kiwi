#ifndef KIWI_GPU_ACC_VECTOR_NAIVE_HEADER_H_
#define KIWI_GPU_ACC_VECTOR_NAIVE_HEADER_H_

#include <cassert>
#include <vector>

class VecNaive {
    std::vector<double> elems;

  public:
    VecNaive(size_t n) : elems(n) {}

    double &operator[](size_t i) { return elems[i]; }
    double operator[](size_t i) const { return elems[i]; }
    size_t size() const { return elems.size(); }

    template <typename Reducer> double reduce(Reducer reducer, double init = 0, int ThreadNum = 0) {
        // clang-format off
        double sum = init;
        for(std::size_t i = 0; i < size(); ++i) {
            sum = reducer(sum, (*this)[i]);
        }
        return sum;
        // clang-format on
    }
};

VecNaive operator+(VecNaive const &u, VecNaive const &v) {
    assert(u.size() == v.size());
    VecNaive sum(u.size());
    for(size_t i = 0; i < u.size(); i++) {
        sum[i] = u[i] + v[i];
    }
    return sum;
}

VecNaive operator*(VecNaive const &u, VecNaive const &v) {
    assert(u.size() == v.size());
    VecNaive sum(u.size());
    for(size_t i = 0; i < u.size(); i++) {
        sum[i] = u[i] * v[i];
    }
    return sum;
}

double dot(VecNaive const &u, VecNaive const &v, int count = 0) {
    assert(u.size() == v.size());
    return (u * v).reduce([](double a, double b) { return a + b; }, 0, count);
}

#endif
