#pragma once
#include <gtest/gtest.h>
#include <iostream>

#include "AST/Builder.h"
#include "AST/TreeOperations.h"

using namespace kiwi;

TEST(AST, Printing)
{
    Root v = Builder<>::value(2);
    Root w = Builder<>::value(3);
    Root x = Builder<>::placeholder("x");

    Root f = Builder<>::binary("+", v, Builder<>::borrow(x));

    Root g = Builder<>::binary("+", w, x);

    print(std::cout, f);
    std::cout << std::endl;

    print(std::cout, g);
    std::cout << std::endl;
}

TEST(AST, Eval)
{
    Root v = Builder<>::value(2);
    Root x = Builder<>::placeholder("x");

    Root f = Builder<>::binary("+", v, x);

    std::unordered_map<std::string, double> ctx;
        ctx["x"] = 2;

    EXPECT_DOUBLE_EQ(full_eval(ctx, f), 4.0);
}


