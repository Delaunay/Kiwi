#ifndef KIWI_GPU_ACC_OPENCL_HEADER_H_
#define KIWI_GPU_ACC_OPENCL_HEADER_H_

#include <CL/cl.h>
//#include <CL/cl.hpp>

#include <vector>

class OpenCL {
  private:
    typedef void (*ErrorHandler)(const char *errinfo, const void *private_info, size_t cb,
                                 void *user_data);

  public:
    OpenCL(size_t default_platform = 0) {
        init_platforms();
        init_devices(default_platform);
        make_context();
    }

    static OpenCL &open_cl(size_t default_platform = 0) {
        static OpenCL instance(default_platform);
        return instance;
    }

    // OpenCl Implementation are called platform
    void init_platforms() {
        cl_uint platformIdCount = 0;
        clGetPlatformIDs(0, nullptr, &platformIdCount);
        platformIds.resize(platformIdCount);
        clGetPlatformIDs(platformIdCount, platformIds.data(), nullptr);

        printf("Platforms:\n");
        char buffer[512];
        for(size_t i = 0; i < platformIds.size(); ++i) {
            size_t size;

            clGetPlatformInfo(platformIds[i], CL_PLATFORM_NAME, 512, buffer, &size);
            printf("- %s\n", buffer);
            clGetPlatformInfo(platformIds[i], CL_PLATFORM_PROFILE, 512, buffer, &size);
            printf("  - %s\n", buffer);
            clGetPlatformInfo(platformIds[i], CL_PLATFORM_VERSION, 512, buffer, &size);
            printf("  - %s\n", buffer);
            clGetPlatformInfo(platformIds[i], CL_PLATFORM_VENDOR, 512, buffer, &size);
            printf("  - %s\n", buffer);
            clGetPlatformInfo(platformIds[i], CL_PLATFORM_EXTENSIONS, 512, buffer, &size);
            printf("  - %s\n", buffer);
        }
        printf("\n");
    }

    // get GPUS
    void init_devices(size_t default_platform = 0) {
        cl_uint deviceIdCount = 0;
        clGetDeviceIDs(platformIds[default_platform], CL_DEVICE_TYPE_ALL, 0, nullptr,
                       &deviceIdCount);
        deviceIds.resize(deviceIdCount);
        clGetDeviceIDs(platformIds[default_platform], CL_DEVICE_TYPE_ALL, deviceIdCount,
                       deviceIds.data(), nullptr);

        printf("Devices:\n");
        char buffer[512];
        for(size_t i = 0; i < deviceIds.size(); ++i) {
            size_t size;

            clGetDeviceInfo(deviceIds[i], CL_DEVICE_NAME, 512, buffer, &size);
            printf("- %s\n", buffer);
            clGetDeviceInfo(deviceIds[i], CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, 512, buffer, &size);
            printf("  - CL_DEVICE_GLOBAL_MEM_CACHE_SIZE     %llu\n", cl_ulong(buffer[0]));
            clGetDeviceInfo(deviceIds[i], CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, 512, buffer, &size);
            printf("  - CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE %d\n", cl_uint(buffer[0]));
            clGetDeviceInfo(deviceIds[i], CL_DEVICE_GLOBAL_MEM_SIZE, 512, buffer, &size);
            printf("  - CL_DEVICE_GLOBAL_MEM_SIZE           %llu\n", cl_ulong(buffer[0]));
            clGetDeviceInfo(deviceIds[i], CL_DEVICE_EXECUTION_CAPABILITIES, 512, buffer, &size);
            printf("  - CL_DEVICE_EXECUTION_CAPABILITIES    %d\n", int(buffer[0]));
            clGetDeviceInfo(deviceIds[i], CL_DEVICE_COMPILER_AVAILABLE, 512, buffer, &size);
            printf("  - CL_DEVICE_COMPILER_AVAILABLE        %d\n", int(buffer[0]));
            clGetDeviceInfo(deviceIds[i], CL_DEVICE_AVAILABLE, 512, buffer, &size);
            printf("  - CL_DEVICE_AVAILABLE                 %d\n", int(buffer[0]));
            clGetDeviceInfo(deviceIds[i], CL_DEVICE_VERSION, 512, buffer, &size);
            printf("  - CL_DEVICE_VERSION                   %s\n", buffer);
            clGetDeviceInfo(deviceIds[i], CL_DRIVER_VERSION, 512, buffer, &size);
            printf("  - CL_DRIVER_VERSION                   %s\n", buffer);
        }
        printf("\n");
    }

    cl_context make_context() {
        cl_platform_id pid = default_platform();
        cl_device_id did   = default_device();

        const cl_context_properties contextProperties[] = {
            CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(pid), 0, 0};

        // An OpenCL context is created with one or more devices. Contexts are used by the OpenCL
        // runtime for managing objects such as command-queues, memory, program and kernel objects
        // and for executing kernels on one or more devices specified in the context.
        cl_context ctx = clCreateContext(contextProperties, 1, &did, handle_error, this, &_error);
        contexts.push_back(ctx);
        return ctx;
    }

    template <typename T> cl_mem allocate_host_memory(cl_context ctx, T *data, std::size_t size) {
        return clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, sizeof(T) * (size),
                              static_cast<void *>(data), &_error);
    }

    template <typename T>
    cl_mem copy_host_memory_to_host_memory(cl_context ctx, T *data, std::size_t size) {
        return clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR | CL_MEM_ALLOC_HOST_PTR,
                              sizeof(T) * (size), static_cast<void *>(data), &_error);
    }

    template <typename T> cl_mem copy_host_memory(cl_context ctx, T *data, std::size_t size) {
        return clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(T) * (size),
                              static_cast<void *>(data), &_error);
    }

    template <typename T> cl_mem use_host_memory(cl_context ctx, T *data, std::size_t size) {
        return clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, sizeof(T) * (size),
                              static_cast<void *>(data), &_error);
    }

    static void handle_error(const char *errinfo, const void *, size_t, void *) {
        printf("GPU Error: %s\n", errinfo);
    }

    void check_error(cl_int error) {
        switch(error) {
        case CL_SUCCESS:
            return;
        default:
            printf("An Error Occured %d\n", error);
        }
    }

    cl_context &default_context() { return contexts[0]; }
    cl_platform_id &default_platform() { return platformIds[0]; }
    cl_device_id &default_device() { return deviceIds[0]; }

  private:
    cl_int _error = CL_SUCCESS;
    std::vector<cl_context> contexts;
    std::vector<cl_platform_id> platformIds;
    std::vector<cl_device_id> deviceIds;
};

#endif
