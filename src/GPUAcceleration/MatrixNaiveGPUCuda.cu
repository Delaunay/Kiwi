#include "MatrixNaiveGPUCuda.h"

#define NOMINMAX
#include <algorithm>

unsigned long closest_power2(unsigned long v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

#ifdef __CUDACC__
__global__ void matrix_mult_kernel(double *u, double *v, double *ret,
                                   int n, int p) {

    const int r = blockIdx.x * blockDim.x + threadIdx.x;
    const int c = blockIdx.y * blockDim.y + threadIdx.y;

    //printf("sr=%d sc=%d (bx=%d, by=%d)\n", r, c, blockDim.x, blockDim.y);

    if (r < n && c < p)
        for(int i = 0; i < n; ++i) {
            ret[c + r * p] += u[i + r * n] * v[c + i * p];
            //printf("(%d %d) u[%d](%4.2f) * v[%d](%4.2f) +=> %4.2f\n", r, c, i + r * n, u[i + r * n],  c+ i * p, v[c + i * p], ret[c + r * p]);
        }
}

void check_cuda_error(cudaError_t e){
    if(e != cudaSuccess) {
      printf("Cuda failure %s:%d: '%s'\n",__FILE__,__LINE__, cudaGetErrorString(e));
    }
}

void check_cuda_error(){
    check_cuda_error(cudaGetLastError());
}


int get_block_num(int n, int bsize){
    return (n + bsize - 1) / bsize;
}

MatrixNaiveCUDA MatrixNaiveCUDA::operator*(MatrixNaiveCUDA &v) {
    assert(u.cols() == v.rows());
    MatrixNaiveCUDA r(rows(), v.cols(), 0);

    /*
    DEBUG("Calling Cuda Code\n");
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, 0);
    DEBUG("MaxGridSize      : %d x %d x %d\n", prop.maxGridSize[0], prop.maxGridSize[1], prop.maxGridSize[2]);
    DEBUG("MaxThreadsDim    : %d x %d x %d\n", prop.maxThreadsDim[0], prop.maxThreadsDim[1], prop.maxThreadsDim[2]);
    DEBUG("MaxThreadPerBlock: %d\n", prop.maxThreadsPerBlock);*/

    // unsigned long closest_power2(unsigned long v)
    size_t xblock_size  = std::min(unsigned long long (32), rows());
    size_t yblock_size  = std::min(unsigned long long (32), cols());
    dim3 threadsPerBlock(xblock_size, yblock_size);
    dim3 numBlocks(get_block_num(r.rows(), xblock_size), get_block_num(r.cols(), yblock_size));

    DEBUG("threadsPerBlock (%lu x %lu)\n", threadsPerBlock.x, threadsPerBlock.y);
    DEBUG("numBlocks (%lu x %lu)\n", numBlocks.x, numBlocks.y);

    cudaError_t e = cudaGetLastError();

    check_cuda_error();
    matrix_mult_kernel<<<numBlocks, threadsPerBlock>>>(&elems[0], &v.elems[0], &r.elems[0], v.rows(),
                                               v.cols());
    check_cuda_error();
    cudaDeviceSynchronize();
    return r;
}
MatrixNaiveCUDA::MatrixNaiveCUDA(size_t r, size_t c, double val) : vrows(r), _rows(r), _cols(c) {

    // elems(r * c, val),
    DEBUG("TEST\n");
    _true_rows = closest_power2(rows());
    _true_cols = closest_power2(cols());
    size_t bytes = std::max(unsigned long long (256), _true_rows * _true_cols * sizeof(double));

    DEBUG("%llu %llu\n", _true_rows, _true_cols);
    DEBUG("Allocate %4.2f Mo %llu\n", float(bytes) / (1024.0 * 1024.0), bytes / sizeof(double));

    check_cuda_error(cudaMallocManaged(&elems, bytes));

    for(size_t i = 0; i < size(); ++i) {
        elems[i] = val;
    }
    for(size_t i = size(); i < bytes / sizeof(double); ++i) {
        elems[i] = 0;
    }

    DEBUG("Initialization\n");
    for(size_t i = 0; i < rows(); ++i) {
        vrows[i] = &elems[i * cols()];
    }
}

MatrixNaiveCUDA::~MatrixNaiveCUDA() { cudaFree(elems); }

MatrixNaiveCUDA::MatrixNaiveCUDA(MatrixNaiveCUDA const &data) :
    vrows(data.vrows), _rows(data._rows), _cols(data._cols) {
    DEBUG("Copying");
    size_t bytes = std::max(unsigned long (256), closest_power2(size() * sizeof(double)));
    check_cuda_error(cudaMallocManaged(&elems, bytes));

    for(size_t i = 0; i < size(); ++i) {
        elems[i] = data.elems[i];
    }
    for(size_t i = size(); i < bytes / sizeof(double); ++i) {
        elems[i] = 0;
    }

    for(size_t i = 0; i < rows(); ++i) {
        vrows[i] = &elems[i * cols()];
    }
}
#endif
