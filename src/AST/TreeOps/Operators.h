#pragma once

#include <iterator>
#include <algorithm>
#include <functional>

#include "../../Types.h"

namespace kiwi {
    using BinaryOperator = std::function<double(double, double)>;
    using BinaryOperatorTable = Dict<String, BinaryOperator>;

    using UnaryOperator = std::function<double(double)> ;
    using UnaryOperatorTable = Dict<String, UnaryOperator>;

    BinaryOperatorTable& binary_table();
    UnaryOperatorTable& unary_table();

    BinaryOperator& binary_operator(const String& name);
    UnaryOperator& unary_operator(const String& name);
}
