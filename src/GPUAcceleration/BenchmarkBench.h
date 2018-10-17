#ifndef KIWI_GPU_ACC_BENCH_BENCH_HEADER_H_
#define KIWI_GPU_ACC_BENCH_BENCH_HEADER_H_

#include <cstdio>
#include <random>

#include <chrono>
#include <ratio>

#include <cmath>
#include <functional>
#include <map>
#include <string>

class TimeIt {
  public:
    using TimePoint       = std::chrono::high_resolution_clock::time_point;
    using Clock           = std::chrono::high_resolution_clock;
    TimePoint const start = Clock::now();

    double stop() {
        TimePoint end = Clock::now();
        std::chrono::duration<double> time_span =
            std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        return time_span.count();
    }
};

struct StatStream {
    StatStream &operator+=(double val) {
        count += 1;
        sum += val;
        sum2 += val * val;
        return *this;
    }

    bool less_than(StatStream const &stats) const { return mean() < stats.mean(); }
    bool greater_than(StatStream const &stats) const { return mean() < stats.mean(); }

    bool equal(StatStream const &) const { return false; }
    bool different(StatStream const &) const { return true; }

    double mean() const { return sum / double(count); }
    double var() const { return sum2 / double(count) - mean() * mean(); }
    double sd() const { return sqrt(var()); }

    double sum        = 0;
    double sum2       = 0;
    std::size_t count = 0;
};

template <typename T> void ignore_unused_variable(const T &) {}

template <typename UserData> class BenchmarkBench {
  public:
    BenchmarkBench(std::size_t repetition_count, std::size_t test_count) :
        repetition_count(repetition_count), test_count(test_count) {}

    template <typename Function> double run(UserData const &data, Function fun) {
        // Do not count the first call
        // call has no effect if the return value is not stored somewhere
        // typename Function::ReturnType volatile result =
        fun(data);

        for(std::size_t i = 0; i < repetition_count; ++i) {
            TimeIt time;
            for(std::size_t j = 0; j < test_count; ++j) {
                fun(data);
            }
            all_time += time.stop();
        }
        return all_time.mean();
    }

  public:
    StatStream all_time;
    std::size_t repetition_count;
    std::size_t test_count;
};

template <typename UserData> class BenchMarkSuite {
  public:
    BenchMarkSuite(char const *bench_name, std::size_t repetition_count, std::size_t test_count) :
        bench_name(bench_name), repetition_count(repetition_count), test_count(test_count) {
        printf("\n");
        add_documentation("Repeat Count", std::to_string(repetition_count));
        add_documentation("Number", std::to_string(test_count));
    }

    void run(char const *test_name, UserData const &data,
             std::function<void(BenchmarkBench<UserData> &, UserData const &)> fun) {
        BenchmarkBench<UserData> bench(repetition_count, test_count);

        if(fun) {
            fun(bench, data);
        } else {
            printf("ERROR\n");
        }

        if(bench.all_time.count > 0) {
            test_results.emplace_back(test_name, bench.all_time);
        } else {
            printf("run() was not called inside the benchmark function\n");
        }
        printf("\r%s: done", test_name);
        fflush(stdout);
    }

    void simple_report() {
        printf("\r");
        printf("%s\n", std::string(66, '=').c_str());
        printf("   %s\n", bench_name);
        printf("%s\n\n", std::string(66, '-').c_str());

        for(auto &key : specs) {
            printf("   %30s: %s\n", key.first.c_str(), key.second.c_str());
        }
        printf("\n");

        std::sort(std::begin(test_results), std::end(test_results),
                  [](std::pair<std::string, StatStream> const &a,
                     std::pair<std::string, StatStream> const &b) {
                      return a.second.mean() > b.second.mean();
                  });

        printf(".%s.\n", std::string(64, '-').c_str());
        printf("| %30s | Runtime (s) |  SD (s) | Count |\n", "Name");
        printf("|%s|\n", std::string(64, '-').c_str());
        for(auto &key : test_results) {
            std::string const &name = key.first;
            StatStream const &stats = key.second;
            printf("| %30s | %11.4f | %7.4f | %5llu |\n", name.c_str(), stats.mean(), stats.sd(),
                   stats.count);
        }
        printf("'%s'\n", std::string(64, '=').c_str());
    }

    void add_documentation(std::string const &name, std::string const &value) {
        specs.emplace_back(name, value);
    }

    template <typename T> void add_documentation(std::string const &name, T const &value) {
        add_documentation(name, std::to_string(value));
    }

  public:
    char const *bench_name;
    std::size_t const repetition_count;
    std::size_t const test_count;

  private:
    std::vector<std::pair<std::string, std::string>> specs;
    std::vector<std::pair<std::string, StatStream>> test_results;
};

#endif
