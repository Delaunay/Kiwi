INCLUDE_DIRECTORIES(../../dependencies/eigen)

#file(GLOB_RECURSE GPU_ACC_SRC *.cpp EXCLUDE MatrixNaiveGPUCuda.cpp)
file(GLOB_RECURSE GPU_ACC_HDS *.h *.inc)

#    SDLMain.cpp
#TARGET_LINK_LIBRARIES(sdl_rendering ast parsing ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY})

SET(GPU_ACC_SRC
    MatrixExpr.cpp
    MatrixNaiveGPUOpenCL.cpp
    OpenCL.cpp
    VectorNaive.cpp
    main.cpp
)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

find_package(cuda)
find_package(OpenCL)
INCLUDE_DIRECTORIES(${OpenCL_INCLUDE_DIR})

SET(${CXX_STANDARD_REQUIRED} ON)
ADD_EXECUTABLE(gpu_acc ${GPU_ACC_SRC} ${GPU_ACC_HDS} MatrixNaiveGPUCuda.cpp)
TARGET_LINK_LIBRARIES(gpu_acc ${OpenCL_LIBRARY})
SET_PROPERTY(TARGET gpu_acc PROPERTY CXX_STANDARD 17)

# SM_60
#CUDA_ADD_EXECUTABLE(gpu_acc_cuda ${GPU_ACC_SRC} ${GPU_ACC_HDS} MatrixNaiveGPUCuda.cu)
#target_compile_features(gpu_acc_cuda PUBLIC cxx_std_11)
#TARGET_LINK_LIBRARIES(gpu_acc_cuda ${OpenCL_LIBRARY})
#set_target_properties(gpu_acc_cuda PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
#set_property(TARGET gpu_acc_cuda PROPERTY CUDA_STANDARD 11)


SET(${CXX_STANDARD_REQUIRED} ON)
ADD_EXECUTABLE(simd simd.cpp)
SET_PROPERTY(TARGET simd PROPERTY CXX_STANDARD 17)
