#include "Operators.h"

namespace kiwi {

    BinaryOperatorTable& binary_table() {
        static BinaryOperatorTable op = {
            { "+", [](double a, double b) { return a + b; } },
            { "-", [](double a, double b) { return a - b; } },
            { "*", [](double a, double b) { return a * b; } },
            { "/", [](double a, double b) { return a / b; } },
        };
        return op;
    }

    UnaryOperatorTable& unary_table() {
        static UnaryOperatorTable op = {
            { "ln",     [](double a) { return std::log(a); } },
            { "exp",    [](double a) { return std::exp(a); } },
            { "sqrt",   [](double a) { return std::sqrt(a); } },
            { "return", [](double a) { return a; } },
        };
        return op;
    }

    BinaryOperator& binary_operator(const std::string& name) {
        return binary_table()[name];
    }

    UnaryOperator& unary_operator(const std::string& name) {
        return unary_table()[name];
    }

}