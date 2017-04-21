#include <gtest/gtest.h>
#include <iostream>

#include "AST/Builder.h"
#include "AST/Printing.h"

using namespace kiwi;

TEST(AST, Printing)
{
    Expr v = Builder::value(2);
    Expr x = Builder::placeholder("x");

    Expr f = Builder::add(v, x);

    print(std::cout, f);
    std::cout << std::endl;

    free(f);
}

TEST(AST, Eval)
{
    Expr v = Builder::value(2);
    Expr x = Builder::placeholder("x");

    Expr f = Builder::add(v, x);

    std::unordered_map<std::string, double> ctx;
        ctx["x"] = 2;

    EXPECT_FLOAT_EQ(full_eval(ctx, f), 4);

    free(f);
}


