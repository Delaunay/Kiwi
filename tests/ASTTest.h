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

    Root f = Builder<>::add(v, Builder<>::borrow(x));

    Root g = Builder<>::add(w, x);

    print(std::cout, f);
    std::cout << std::endl;

    print(std::cout, g);
    std::cout << std::endl;


    // def sqr(x):
    //     return x * x

}


TEST(AST, Function)
{
    // def sqr(x):
    //     return x * x

    Root x = Builder<>::placeholder("x");
    Root body = Builder<>::mult(x, Builder<>::borrow(x));

    Root sqr = Builder<>::function("sqr", body);
         static_cast<Function*>(sqr.get())->args.push_back("x");

    // add function to context
    Context ctx = {
      {"sqr", sqr.get()}
    };


    // print function
    std::cout << "decl: ";
    print(std::cout, sqr);
    std::cout << std::endl;

    // call function sqr(2)
    Root val  = Builder<>::value(2);
    Root sqr2 = Builder<>::call("sqr", {val.take_ownership()});


    std::cout << "call: ";
    print(std::cout, sqr2);
    std::cout << std::endl;

    full_eval(ctx, sqr2);
}

TEST(AST, Eval)
{
    Root v = Builder<>::value(2);
    Root x = Builder<>::placeholder("x");

    Root f = Builder<>::add(v, x);

    Context ctx;
        ctx["x"] = v.get();

    EXPECT_DOUBLE_EQ(full_eval(ctx, f), 4.0);
}


