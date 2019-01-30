#include <algorithm>
#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

#define DEBUG(M) std::cout << M

template<typename T>
T print_value(T v) {
    DEBUG(v) << std::endl;
    return v;
}

#define __FUNCSIG__ __FUNCTION__

template <typename T, typename Impl> struct Expr {
    std::future<T> run() {
        DEBUG("Calling run: ") << __FUNCSIG__  << std::endl;
        return static_cast<Impl&>(*this).run();
    }

    operator Impl& () { return static_cast<Impl&>(*this); }
    operator const Impl& () const { return static_cast<const Impl&>(*this); }
private:
};

template<typename T>
T wait_and_get(std::future<T>& f) {
    f.wait();
    return f.get();
}

// Data Provider, return 1 output of Type T
template <typename T>
struct DataProvider : public Expr<T, DataProvider<T>> {
    DataProvider(T v) : value(v) {}

    std::future<T> run() {
        DEBUG("Providing data") << std::endl;
        std::promise<T> p;
        p.set_value(value);
        return p.get_future();
    }

    T value;
};


// Combinator, takes 2 inputs and return 1 output both of type T
template <typename T, typename Expr1, typename Expr2>
struct Add : public Expr<T, Add<T, Expr1, Expr2>>{
    Add(Expr<T, Expr1>const & a, Expr<T, Expr2>const & b):
        lhs(a), rhs(b)
    {}

    static T eval(std::future<T> a, std::future<T> b){
        return wait_and_get(a) + wait_and_get(b);
    }

    std::future<T> run() {
        DEBUG("Adding Numbers") << std::endl;
        return std::async(Add::eval, lhs.run(), rhs.run());
    }

    Expr<T, Expr1> lhs;
    Expr<T, Expr2> rhs;
};


template <typename I, typename O, typename Uexpr>
struct StaticCast : public Expr<O, StaticCast<I, O, Uexpr>> {
    StaticCast(Expr<I, Uexpr>const& a) :
        expr(a)
    {}

    using EvalFunctionType = std::function<O(std::future<I>)>;

    static O eval(std::future<I>  a){
        return static_cast<O>(wait_and_get(a));
    }

    std::future<O> run() {
        DEBUG("Casting Result") << std::endl;
        return std::async(std::launch::async, StaticCast::eval, expr.run());
    }

    Expr<I, Uexpr> expr;
};

template<typename T, typename Expr1, typename Expr2>
Add<T, Expr1, Expr2> add(Expr<T, Expr1>const& lhs, Expr<T, Expr2>const& rhs) {
    return Add<T, Expr1, Expr2>(lhs, rhs);
}

template<typename T, typename Uexpr>
StaticCast<T, int, Uexpr> to_int(Expr<T, Uexpr>const& expr) {
    return StaticCast<T, int, Uexpr>(expr);
}

DataProvider<double> provide_double(double a) {
    return DataProvider<double>(a);
}


int main() {

    StaticCast<double, int, Add<double, DataProvider<double>, DataProvider<double>>> job = to_int(add(provide_double(2), provide_double(3)));

    std::future<int> result = job.run();

    result.wait();

    std::cout << result.get() << std::endl;

    return 0;
}
