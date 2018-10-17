#ifndef KIWI_GPU_ACC_KERNEL_MANAGER_HEADER_H_
#define KIWI_GPU_ACC_KERNEL_MANAGER_HEADER_H_

#include "OpenCL.h"
#include <unordered_map>

class KernelManager {
  private:
    cl_context &ctx;
    std::unordered_map<std::string, cl_program> programs;

  public:
    KernelManager(cl_context &ctx) : ctx(ctx) {}

    cl_program compile_code(std::string const &code) {
        char const *kernel[1] = {code.c_str()};
        size_t length[1]      = {code.size()};

        printf("Build program\n");
        cl_int error;
        cl_device_id device;
        clGetContextInfo(ctx, CL_CONTEXT_DEVICES, sizeof(cl_device_id), &device, nullptr);

        cl_program program = clCreateProgramWithSource(ctx, 1, kernel, length, &error);
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
            printf("%s\n", log);
            free(log);
        }

        return program;
    }

    template <typename... Args>
    cl_kernel call_kernel(cl_program program, std::string const &name, Args... args) {
        cl_int error;
        cl_kernel kernel = clCreateKernel(program, name.c_str(), &error);
        check_error(error);
        return call_kernel(kernel, 0, args...);
    }

    template <typename T, typename... Args>
    cl_kernel call_kernel(cl_kernel kernel, cl_int pos, T value, Args... args) {
        check_error(clSetKernelArg(kernel, pos, sizeof(T), &value));
        return call_kernel(kernel, pos + 1, args...);
    }

    cl_kernel call_kernel(cl_kernel k, cl_int) { return k; }

    void check_error(cl_int error) {
        switch(error) {
        case CL_SUCCESS:
            return;
        default:
            printf("An Error Occured %d\n", error);
        }
    }

    /*
        char const *kernel_rows_depth_cols = CL_CODE(
            __kernel void kernel_rows_depth_cols(__global double *u, __global double *v,
                                                 __global double *ret, int rows, int depth, int
       cols) { const int r        = get_global_id(0); const int d        = get_global_id(1); const
       int u_offset = r * depth; const int v_offset = d * cols; const int r_offset = r * cols;
                for(int c = 0; c < cols; ++c) {
                    //  rows x cols   =     rows x depth   *   depth x cols
                    ret[c + r_offset] += u[d + u_offset] * v[c + v_offset];
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

        // #define CL_BUILD_PROGRAM_FAILURE                    -11

        DEBUG("Build Kernel \n");
        cl_kernel kernel = check_error(error);*/
};

#endif
