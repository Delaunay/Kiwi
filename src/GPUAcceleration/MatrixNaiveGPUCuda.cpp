#include "MatrixNaiveGPUCuda.h"

#define NOMINMAX
#include <algorithm>

#ifdef __CUDACC__
#else
MatrixNaiveCUDA MatrixNaiveCUDA::operator*(MatrixNaiveCUDA &v) {
    assert(u.cols() == v.rows());
    MatrixNaiveCUDA r(rows(), v.cols());
    printf("No CUDA Code\n");fflush(stdout);
    return r;
}

MatrixNaiveCUDA::MatrixNaiveCUDA(size_t r, size_t c, double val) : vrows(r), _rows(r), _cols(c) {
    printf("No CUDA Code\n");fflush(stdout);
}

MatrixNaiveCUDA::~MatrixNaiveCUDA() {
    printf("No CUDA Code\n");fflush(stdout);
}

MatrixNaiveCUDA::MatrixNaiveCUDA(MatrixNaiveCUDA const &data){
    printf("No CUDA Code\n");fflush(stdout);
}
#endif
