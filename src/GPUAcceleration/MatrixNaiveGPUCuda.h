#ifndef KIWI_GPU_ACC_MATRIX_NAIVE_CUDA_HEADER_H_
#define KIWI_GPU_ACC_MATRIX_NAIVE_CUDA_HEADER_H_

#include <cassert>
#include <vector>

#include "OpenCL.h"
#include <cuda.h>

#define CL_CODE(X) #X
#define DEBUG(frmt, ...) printf(frmt, __VA_ARGS__)
#undef DEBUG
#define DEBUG(...)

class MatrixNaiveCUDA {
    double *elems;
    std::vector<double *> vrows;
    size_t _rows;
    size_t _cols;
    size_t _true_rows;
    size_t _true_cols;

  public:
    MatrixNaiveCUDA(size_t r, size_t c, double val = 0);

    ~MatrixNaiveCUDA();

    MatrixNaiveCUDA(MatrixNaiveCUDA &&data) :
        elems(data.elems), vrows(std::move(data.vrows)), _rows(data._rows), _cols(data._cols) {
        data.elems = nullptr;
        DEBUG("Move Ctor");
    }

    size_t aligned_cols() const { return _true_cols; }
    size_t aligned_rows() const { return _true_rows; }

    MatrixNaiveCUDA(MatrixNaiveCUDA const &data);

    size_t size() const { return rows() * cols(); }

    double &operator()(size_t r, size_t c) { return vrows[r][c]; }
    double operator()(size_t r, size_t c) const { return vrows[r][c]; }

    size_t rows() const { return _rows; }
    size_t cols() const { return _cols; }

  public:
    void check_error(cl_int error) {
        switch(error) {
        case CL_SUCCESS:
            return;
        default:
            DEBUG("An Error Occured %d\n", error);
        }
    }

    MatrixNaiveCUDA operator*(MatrixNaiveCUDA &v);

    void print() {
        printf("(%llu x %llu)\n", rows(), cols());
        for(size_t r = 0; r < rows(); ++r) {
            for(size_t c = 0; c < cols(); ++c) {
                printf("%4.2f ", (*this)(r, c));
            }
            printf("\n");
        }
    }
};

#endif
