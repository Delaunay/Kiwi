#ifndef KIWI_GPU_ACC_MATRIX_NAIVE_OPENCL_HEADER_H_
#define KIWI_GPU_ACC_MATRIX_NAIVE_OPENCL_HEADER_H_

#include <cassert>
#include <vector>

#include "KernelManager.h"
#include "OpenCL.h"

#define CL_CODE(X) #X
#undef DEBUG
#define DEBUG(frmt, ...) printf(frmt, __VA_ARGS__)
#undef DEBUG
#define DEBUG(...)

// The Matrix data is both in Host memory and device memory for simplicity
class MatrixNaiveOpenCL {
    std::vector<double> elems;
    std::vector<double *> vrows;
    cl_context ctx;
    cl_mem buffer;
    cl_int last_error;
    size_t _rows;
    size_t _cols;

  public:
    MatrixNaiveOpenCL(cl_context ctx, size_t r, size_t c, double val = 0) :
        elems(r * c, val), vrows(r), ctx(ctx), _rows(r), _cols(c) {
        DEBUG("Initialization\n");
        fflush(stdout);
        for(size_t i = 0; i < rows(); ++i) {
            vrows[i] = &elems[i * cols()];
        }

        buffer =
            clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(double) * (size()),
                           static_cast<void *>(&elems[0]), &last_error);
        check_error(last_error);
    }

    MatrixNaiveOpenCL(MatrixNaiveOpenCL const &) = default;

    ~MatrixNaiveOpenCL() { clReleaseMemObject(buffer); }

    size_t size() const { return elems.size(); }

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

    MatrixNaiveOpenCL operator*(MatrixNaiveOpenCL &v) {
        assert(cols() == v.rows());
        DEBUG("Matrix Mult \n");

        // Make OpenCL use Host Memory
        // Slower but avoid copying betwen RAM and GPU Memory
        // OpenCL implementations are allowed to cache the buffer contents pointed to by host_ptr in
        // device memory. This cached copy can be used when kernels are executed on a device.
        OpenCL &cl = OpenCL::open_cl();
        MatrixNaiveOpenCL r(ctx, rows(), v.cols(), 0);
        cl_context &ctx      = cl.default_context();
        cl_device_id &device = cl.default_device();
        cl_int error;

        /*
        char const *kernel_rows_depth_cols = CL_CODE(__kernel void kernel_rows_depth_cols(
            __global double *u, __global double *v, __global double *ret, int rows, int depth,
            int cols) {
            const int r        = get_global_id(0);
            const int d        = get_global_id(1);
            const int u_offset = r * depth;
            const int v_offset = d * cols;
            const int r_offset = r * cols;
            for(int c = 0; c < cols; ++c) {
                //  rows x cols   =     rows x depth   *   depth x cols
                ret[c + r_offset] += u[d + u_offset] * v[c + v_offset];
            }
        });

        char const *kernel_rows_cols_depth = CL_CODE(__kernel void kernel_rows_cols_depth(
            __global double *u, __global double *v, __global double *ret, int n, int p) {
            const int r    = get_global_id(0);
            const int c    = get_global_id(1);
            ret[c + r * p] = 0;

            for(int i = 0; i < n; ++i) {
                ret[c + r * p] += u[i + r * n] * v[c + i * p];
            }
        });

        char const *print_code = CL_CODE(__kernel void print_mat(__global double *u, int n, int p) {
            //#define DEBUG(frmt, ...) printf(frmt, __VA_ARGS__)
            printf("GPU Debug Mat Size: (%d x %d)\n", n, p);
            for(int r = 0; r < n; ++r) {
                for(int c = 0; c < p; ++c) {
                    printf("(%d, %d) [%d] %4.2f |", c, r, c + r * p, u[c + r * p]);
                }
                printf("\n");
            }
        });

        char const *kernels[2] = {kernel_rows_depth_cols, print_code};
        size_t lengths[2]      = {strlen(kernel_rows_depth_cols), strlen(print_code)};

        DEBUG("Build program \n");

        cl_program program = clCreateProgramWithSource(ctx, 2, kernels, lengths, &error);
        check_error(error);

        error = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
        check_error(error);
        if(error == CL_BUILD_PROGRAM_FAILURE) {
            // Determine the size of the log
            size_t log_size;
            clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size);

            // Allocate memory for the log
            char *log = reinterpret_cast<char *>(malloc(log_size));

            // Get the log
            clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log, nullptr);

            // Print the log
            DEBUG("%s\n", log);
            free(log);
        }*/

        // #define CL_BUILD_PROGRAM_FAILURE                    -11

        // runtime: 4.3922
        char const *kernel_parallel_2 =
            CL_CODE(__kernel void fun_fun(__global double *u, __global double *v,
                                          __global double *ret, int rows, int depth, int cols) {
                const int r        = get_global_id(0);
                const int d        = get_global_id(1);
                const int u_offset = r * depth;
                const int v_offset = d * cols;
                const int r_offset = r * cols;
                for(int c = 0; c < cols; ++c) {
                    //   rows x cols   =   rows x depth  *  depth x cols
                    ret[c + r_offset] += u[d + u_offset] * v[c + v_offset];
                }
            });

        // runtime: 6.3641
        char const *kernel_parallel_1 =
            CL_CODE(__kernel void fun_fun(__global double *u, __global double *v,
                                          __global double *ret, int rows, int depth, int cols) {
                const int r        = get_global_id(0);
                const int u_offset = r * depth;
                const int r_offset = r * cols;
                for(int d = 0; d < depth; ++d) {
                    const int v_offset = d * cols;
                    for(int c = 0; c < cols; ++c) {
                        //   rows x cols   =   rows x depth  *  depth x cols
                        ret[c + r_offset] += u[d + u_offset] * v[c + v_offset];
                    }
                }
            });

        // runtime: 2.4746
        char const *kernel_parallel_3 =
            CL_CODE(__kernel void fun_fun(__global double *u, __global double *v,
                                          __global double *ret, int rows, int depth, int cols) {
                const int r        = get_global_id(0);
                const int d        = get_global_id(1);
                const int c        = get_global_id(2);
                const int u_offset = r * depth;
                const int v_offset = d * cols;
                const int r_offset = r * cols;
                //   rows x cols   =   rows x depth  *  depth x cols
                ret[c + r_offset] += u[d + u_offset] * v[c + v_offset];
            });

        char const *kernel_parallel_pipeline = CL_CODE( //
            __kernel void col_kernel(__global double *u, __global double *v, __global double *ret,
                                     int rows, int depth, int cols, int r, int u_offset,
                                     int r_offset, int d) {
                const int v_offset = d * cols;
                for(int c = 0; c < cols; ++c) {
                    //   rows x cols   =   rows x depth  *  depth x cols
                    ret[c + r_offset] += u[d + u_offset] * v[c + v_offset];
                }
            }

            __kernel void fun_fun(__global double *u, __global double *v, __global double *ret,
                                  int rows, int depth, int cols) {
                const int r        = get_global_id(0);
                const int u_offset = r * depth;
                const int r_offset = r * cols;

                for(int d = 0; d < depth; ++d) {
                    col_kernel(u, v, ret, rows, depth, cols, r, u_offset, r_offset, d);
                }
            });

        int mrows  = int(rows());
        int mdepth = int(v.rows());
        int mcols  = int(cols());

        KernelManager manager(ctx);
        cl_program program = manager.compile_code(kernel_parallel_2);
        cl_kernel kernel   = manager.call_kernel(program, "fun_fun", buffer, v.buffer, r.buffer,
                                               mrows, mdepth, mcols);

        cl_command_queue queue = clCreateCommandQueue(ctx, device, 0, &error);
        check_error(error);

        std::size_t offset[3] = {0};

        DEBUG("%llu, %llu\n", r.rows(), r.cols());

        //
        // size_t ksize[] = {r.rows(), v.rows(), 1};
        // clEnqueueNDRangeKernel(queue, kernel, 2, offset, ksize, nullptr, 0, nullptr, nullptr);

        // Parallel 1
        // size_t ksize[] = {r.rows(), 0, 0};

        // Parallel 3
        // size_t ksize[] = {r.rows(), v.rows(), v.cols()};

        // Parallel 2
        size_t ksize[] = {r.rows(), v.rows(), 0};
        clEnqueueNDRangeKernel(queue, kernel, 2, offset, ksize, nullptr, 0, nullptr, nullptr);

        DEBUG("Wait \n");

        check_error(clEnqueueReadBuffer(queue, r.buffer, CL_TRUE, 0, sizeof(double) * r.size(),
                                        static_cast<void *>(&r.elems[0]), 0, nullptr, nullptr));
        DEBUG("Read Buffer\n");
        check_error(clFinish(queue));
        DEBUG("done\n");

        // Cleanup
        clReleaseCommandQueue(queue);
        clReleaseKernel(kernel);
        clReleaseProgram(program);
        return r;
    }

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
