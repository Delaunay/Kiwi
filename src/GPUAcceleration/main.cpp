//#define EIGEN_VECTORIZE_AVX
#define EIGEN_DONT_VECTORIZE
#include <Eigen/Dense>

#include "VectorExpression.h"
#include "VectorExpressionMP.h"
#include "VectorNaive.h"

#include "MatrixNaive.h"
#include "MatrixNaiveGPUCuda.h"
#include "MatrixNaiveGPUOpenCL.h"

#include "BenchmarkBench.h"
#include "OpenCL.h"

#define VSIZE 1000000
#define TEST_SIZE 3
#define TEST_REP 10

template <typename VectorType> struct RefData {
    VectorType a;
    VectorType b;
    VectorType c;
};

template <typename VectorType>
RefData<VectorType> make_reference_data(size_t v_size, std::vector<double> const &data) {
    VectorType a(v_size);
    VectorType b(v_size);
    VectorType c(v_size);

    for(std::size_t i = 0; i < v_size; ++i) {
        a[i] = data[i];
        b[i] = data[i + v_size];
        c[i] = data[i + v_size * 2];
    }
    return {a, b, c};
}

template <typename VectorType, typename TestFunction> struct TestVectorType {
    TestVectorType(std::vector<double> const &data, char const *message) {
        BenchmarkBench<RefData<VectorType>>(TEST_SIZE, TEST_REP)
            .run(make_reference_data<VectorType>(VSIZE, data), TestFunction(), message);
    }
};

template <typename VectorType> struct VectorSumTest {
    using ReturnType = VectorType;
    ReturnType operator()(RefData<VectorType> const &data) {
        return data.a + data.b + data.c + data.a + data.b + data.c;
    }
};

template <typename VectorType> struct VectorDotTest {
    using ReturnType = double;
    ReturnType operator()(RefData<VectorType> const &data) {
        return dot(data.a * data.b * data.c + data.b, (data.a + data.b + data.c) * data.a);
    }
};

// int main() {}

#define TEST
#ifdef TEST

int main() {
    /*
    $ ./gpu_acc.exe
    VSIZE          10000000
    sizeof(double) 8
    Data           228.88 Mo
    Temporaries    152.59 Mo
    Total          610.35 Mo

       Naive Average : 13.3591 seconds
        Expr Average : 7.1835 seconds
     ExprMp1 Average : 7.1187 seconds
     ExprMp2 Average : 6.7064 seconds
     ExprMp4 Average : 6.8284 seconds
     ExprMp6 Average : 7.0662 seconds
     ExprMp8 Average : 7.1505 seconds
    ExprMp12 Average : 7.5567 seconds
    */

    OpenCL::open_cl(1);

    std::vector<double> data(VSIZE * 3);

    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    auto dice = [&]() { return distribution(generator); };

    for(std::size_t i = 0; i < VSIZE * 3; ++i) {
        data[i] = dice();
    }

    double vector_size = VSIZE * sizeof(double) / (1024.0 * 1024.0);
    // Reference Data + Data copied to the individual vectors + temporaries
    double total_size = vector_size * 3 * 2;
    double temp_size  = vector_size * 2.0;

    printf("# Vector add\n");
    printf("------------\n");
    printf("VSIZE          %u       \n", VSIZE);
    printf("sizeof(double) %lld     \n", sizeof(double));
    printf("Data           %4.2f Mo \n", vector_size * 3.0);
    printf("Temporaries    %4.2f Mo \n", temp_size);
    printf("Total          %4.2f Mo \n", total_size + temp_size);

// clang-format off
#define CPU_DOT
#define CPU_SUM
#define GPU_MATRIX_MULT

#ifdef CPU_VECTOR_SUM
    printf("\n# CPU Vector (sum)\n");
    printf("# ----------------------------------------------------------------\n");
    fflush(stdout);

    TestVectorType<VecNaive    , VectorSumTest<VecNaive>>     (data, "   Naive Average ");
    TestVectorType<VecExpr     , VectorSumTest<VecExpr>>      (data, "    Expr Average ");
    TestVectorType<VecExprMp<1>, VectorSumTest<VecExprMp<1>>> (data, " ExprMp1 Average ");
    TestVectorType<VecExprMp<2>, VectorSumTest<VecExprMp<2>>> (data, " ExprMp2 Average ");
    TestVectorType<VecExprMp<4>, VectorSumTest<VecExprMp<4>>> (data, " ExprMp4 Average ");
    TestVectorType<VecExprMp<8>, VectorSumTest<VecExprMp<8>>> (data, " ExprMp8 Average ");
    TestVectorType<VecExprMp<12>,VectorSumTest<VecExprMp<12>>>(data, "ExprMp12 Average ");
#endif

#ifdef CPU_VECTOR_DOT
    printf("\n# CPU Vector (dot)\n");
    printf("# ----------------------------------------------------------------\n");
    fflush(stdout);

    TestVectorType<VecNaive    , VectorDotTest<VecNaive>>     (data, "   Naive Average ");
    TestVectorType<VecExpr     , VectorDotTest<VecExpr>>      (data, "    Expr Average ");
    TestVectorType<VecExprMp<1>, VectorDotTest<VecExprMp<1>>> (data, " ExprMp1 Average ");
    TestVectorType<VecExprMp<2>, VectorDotTest<VecExprMp<2>>> (data, " ExprMp2 Average ");
    TestVectorType<VecExprMp<4>, VectorDotTest<VecExprMp<4>>> (data, " ExprMp4 Average ");
    TestVectorType<VecExprMp<8>, VectorDotTest<VecExprMp<8>>> (data, " ExprMp8 Average ");
    TestVectorType<VecExprMp<12>,VectorDotTest<VecExprMp<12>>>(data, "ExprMp12 Average ");
#endif

#ifdef GPU_MATRIX_MULT
    printf("Eigen SIMD instructuions: %s \n", Eigen::SimdInstructionSetsInUse());

    fflush(stdout);

    using Storage = AVX512Storage;
    using MatrixNaive2D = MatrixNaive<MemoryMapper2D<double>, AVX512Storage>;
    using MatrixNaive1D = MatrixNaive<MemoryMapper1D<double>, AVX512Storage>;
    using MatrixNaive2DNA = MatrixNaive<MemoryMapper2D<double>, std::vector<double>>;

    using MatrixMultFunction = MatrixNaive2D(*)(MatrixNaive2D const &,MatrixNaive2D const &);
    using BenchMarkFunction = std::function<void(BenchmarkBench<int> &, int const &)>;

    size_t size = size_t(sqrt(VSIZE));
    double matrix_size = size * size * sizeof(double) / (1024.0 * 1024.0);

    std::function<BenchMarkFunction(MatrixMultFunction)> bench_function_maker = [&](MatrixMultFunction matrix_mult){
        return [&](BenchmarkBench<int>& bench, int const&){
            MatrixNaive2D a(size, size, 2.0);
            MatrixNaive2D b(size, size, 1.0);

            bench.run(1, [&](int){
                MatrixNaive2D c = matrix_mult(a, b); // fails here
                volatile double s = sum(c);
                ignore_unused_variable(s);
            });
        };
    };


#define MAKE_BENCH_FUN(mult, MatrixType)\
    [&](BenchmarkBench<int>& bench, int const&){\
            MatrixType a(size, size, 2.0);\
            MatrixType b(size, size, 1.0);\
            bench.run(1, [&](int){\
                MatrixType c = mult(a, b);\
                volatile double s = sum(c);\
                ignore_unused_variable(s);\
            });\
        }


#define NAME(X, Y) X
    double total_mat_size = matrix_size * 3;

//#define KIWI_PARALLEL_MULT_1D

#ifdef KIWI_SEQUENTIAL_MULT
#define MMST <MemoryMapper2D<Storage>>
    BenchMarkSuite<int> benchsuite(
                NAME("CPU Matrix Naive Matrix Multiply Sequential ", MMST), TEST_SIZE, TEST_REP);

    benchsuite.add_documentation("VSIZE", VSIZE);
    benchsuite.add_documentation("size", size);
    benchsuite.add_documentation("sizeof(double)", sizeof(double));
    benchsuite.add_documentation("Matrix Size (Mo)", matrix_size);
    benchsuite.add_documentation("Data (Mo)", total_mat_size);

    benchsuite.run("(rows x cols x depth)", 1, MAKE_BENCH_FUN(matrix_mult_rows_cols_depth MMST));
    benchsuite.run("(cols x rows x depth)", 1, MAKE_BENCH_FUN(matrix_mult_cols_rows_depth MMST));

    benchsuite.run("(depth x cols x rows)", 1, MAKE_BENCH_FUN(matrix_mult_depth_cols_rows MMST));
    benchsuite.run("(depth x rows x cols)", 1, MAKE_BENCH_FUN(matrix_mult_depth_rows_cols MMST));

    benchsuite.run("(cols x depth x rows)", 1, MAKE_BENCH_FUN(matrix_mult_cols_depth_rows MMST));
    benchsuite.run("(rows x depth x cols)", 1, MAKE_BENCH_FUN(matrix_mult_rows_depth_cols MMST));

    benchsuite.simple_report();
#undef MMST
#endif
//*/
#ifdef KIWI_PARALLEL_MULT_2D
#define MMST <MemoryMapper2D<Storage>>
    BenchMarkSuite<int> benchsuite_parallel(
                NAME("CPU Matrix Naive Matrix Multiply Parallel ", MMST), TEST_SIZE, TEST_REP);

    benchsuite_parallel.add_documentation("VSIZE", VSIZE);
    benchsuite_parallel.add_documentation("size", size);
    benchsuite_parallel.add_documentation("sizeof(double)", sizeof(double));
    benchsuite_parallel.add_documentation("Matrix Size (Mo)", matrix_size);
    benchsuite_parallel.add_documentation("Data (Mo)", total_mat_size);

    benchsuite_parallel.run("(rows x cols x depth)", 1, MAKE_BENCH_FUN(matrix_mult_parallel_rows_cols_depth MMST, MatrixNaive2D));
    benchsuite_parallel.run("(cols x rows x depth)", 1, MAKE_BENCH_FUN(matrix_mult_parallel_cols_rows_depth MMST, MatrixNaive2D));

    benchsuite_parallel.run("(depth x cols x rows)", 1, MAKE_BENCH_FUN(matrix_mult_parallel_depth_cols_rows MMST, MatrixNaive2D));
    benchsuite_parallel.run("(depth x rows x cols)", 1, MAKE_BENCH_FUN(matrix_mult_parallel_depth_rows_cols MMST, MatrixNaive2D));

    benchsuite_parallel.run("(cols x depth x rows)", 1, MAKE_BENCH_FUN(matrix_mult_parallel_cols_depth_rows MMST, MatrixNaive2D));
    benchsuite_parallel.run("(rows x depth x cols)", 1, MAKE_BENCH_FUN(matrix_mult_parallel_rows_depth_cols MMST, MatrixNaive2D));
    benchsuite_parallel.run("Eigen", 1, [&](BenchmarkBench<int>& bench, int const&){
        Eigen::MatrixXd a = Eigen::MatrixXd::Constant(Eigen::Index(size), Eigen::Index(size), 2.0);
        Eigen::MatrixXd b = Eigen::MatrixXd::Constant(Eigen::Index(size), Eigen::Index(size), 1.0);

        bench.run(1, [&](int){
            Eigen::MatrixXd c = (a * b).eval();
            volatile double s = c.array().sum();
            ignore_unused_variable(s);
        });
    });//*/

    benchsuite_parallel.simple_report();
#undef MMST
#endif
#ifdef KIWI_PARALLEL_MULT_1D
#define MMST <MatrixNaive1D>
    BenchMarkSuite<int> benchsuite_mapper(
                NAME("CPU Matrix Naive Matrix Memory Mapper", MMST), TEST_SIZE, TEST_REP);

    benchsuite_mapper.add_documentation("VSIZE", VSIZE);
    benchsuite_mapper.add_documentation("size", size);
    benchsuite_mapper.add_documentation("sizeof(double)", sizeof(double));
    benchsuite_mapper.add_documentation("Matrix Size (Mo)", matrix_size);
    benchsuite_mapper.add_documentation("Data (Mo)", total_mat_size);

    benchsuite_mapper.run("(rows x depth x cols) 1D Al", 1, MAKE_BENCH_FUN(matrix_mult_parallel_rows_depth_cols MMST, MatrixNaive1D));
    benchsuite_mapper.run("(rows x depth x cols) 2D Al", 1, MAKE_BENCH_FUN(matrix_mult_parallel_rows_depth_cols<MatrixNaive2D>, MatrixNaive2D));
    benchsuite_mapper.run("(rows x depth x cols) 2D ", 1, MAKE_BENCH_FUN(matrix_mult_parallel_rows_depth_cols<MatrixNaive2DNA>, MatrixNaive2DNA));

    benchsuite_mapper.run("Eigen", 1, [&](BenchmarkBench<int>& bench, int const&){
        Eigen::MatrixXd a = Eigen::MatrixXd::Constant(Eigen::Index(size), Eigen::Index(size), 2.0);
        Eigen::MatrixXd b = Eigen::MatrixXd::Constant(Eigen::Index(size), Eigen::Index(size), 1.0);

        bench.run(1, [&](int){
            Eigen::MatrixXd c = (a * b).eval();
            volatile double s = c.array().sum();
            ignore_unused_variable(s);
        });
    });//*/

    benchsuite_mapper.simple_report();
#endif

    BenchMarkSuite<int> benchsuite_gpu("GPU Matrix Naive", TEST_SIZE, TEST_REP);

    benchsuite_gpu.add_documentation("VSIZE", VSIZE);
    benchsuite_gpu.add_documentation("size", size);
    benchsuite_gpu.add_documentation("sizeof(double)", sizeof(double));
    benchsuite_gpu.add_documentation("Matrix Size (Mo)", matrix_size);
    benchsuite_gpu.add_documentation("Data (Mo)", total_mat_size);


    benchsuite_gpu.run("OpenCL", 1, [&](BenchmarkBench<int>& bench, int const&){
        OpenCL &cl = OpenCL::open_cl();
        cl_context &ctx      = cl.default_context();

        MatrixNaiveOpenCL a(ctx, size, size, 2.0);
        MatrixNaiveOpenCL b(ctx, size, size, 1.0);

        bench.run(1, [&](int){
            volatile MatrixNaiveOpenCL c = a * b;
        });
    });

    benchsuite_gpu.run("Eigen", 1, [&](BenchmarkBench<int>& bench, int const&){
        Eigen::MatrixXd a = Eigen::MatrixXd::Constant(Eigen::Index(size), Eigen::Index(size), 2.0);
        Eigen::MatrixXd b = Eigen::MatrixXd::Constant(Eigen::Index(size), Eigen::Index(size), 1.0);

        bench.run(1, [&](int){
            Eigen::MatrixXd c = (a * b).eval();
            volatile double s = c.array().sum();
            ignore_unused_variable(s);
        });
    });


    benchsuite_gpu.simple_report();
    /*
    {

    }
    {
        MatrixNaiveCUDA a(size, size, 2.0);
        MatrixNaiveCUDA b(size, size, 1.0);

        BenchmarkBench<int>(TEST_SIZE, TEST_REP)
                .run(1, [&](int){
            volatile MatrixNaiveCUDA c = a * b;
        }, "GPU MatrixNaiveCuda");
    }*/




#endif

    // clang-format on

    return 0;
}
#endif

/*





for(std::size_t i = 0; i < d.size(); ++i) {
    printf("%4.2f, ", d[i]);
}

// clang-format off
// #define _AMD64_
//#include <sdkddkver.h>
//#include <Windows.h>
//#include <Psapi.h>
// clang-format on

*/

/*
class RamUsage {
  public:
    RamUsage() {}

    std::size_t get_vmemory_usage() {
        // GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
        // return pmc.PrivateUsage;
    }

    std::size_t get_pmemory_usage() {
        // GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
        // return pmc.WorkingSetSize;
    }

  private:
    // PROCESS_MEMORY_COUNTERS_EX pmc;
};*/
