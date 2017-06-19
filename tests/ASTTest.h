#pragma once
#include <gtest/gtest.h>
#include <iostream>

#include "AST/Builder.h"
#include "AST/TreeOperations.h"

using namespace kiwi;

typedef generic::Root<Expression> Root;
typedef Builder<Root, LightAST> RBuilder;

TEST(AST, Printing)
{
    Root v = RBuilder::value(2.0);
    Root w = RBuilder::value(3.0);
    Root x = RBuilder::placeholder("x");

    Root g = RBuilder::add(w, x);
    Root f = RBuilder::add(v, RBuilder::borrow(x));

    print(std::cout, f);
    std::cout << std::endl;

    print(std::cout, g);
    std::cout << std::endl;
}


TEST(AST, Function)
{
    // def sqr(x):
    //     return x * x

    Root x    = RBuilder::placeholder("x");
    Root body = RBuilder::mult(x, RBuilder::borrow(x));
    Root sqr  = RBuilder::function("sqr", body);
         static_cast<LightAST::Function*>(sqr.get())->args.push_back("x");

    // add function to context
    Context ctx = {
      {"sqr", sqr.get()}
    };

    // print function
    std::cout << "decl: \n"; print(std::cout, sqr);
    std::cout << std::endl;

    // call function sqr(2)
    Root val  = RBuilder::value(3.0);
    Root sqr2 = RBuilder::call("sqr", {val.take_ownership()});

    std::cout << "call: "; print(std::cout, sqr2);
    std::cout << std::endl;

    EXPECT_DOUBLE_EQ(full_eval(ctx, sqr2), 9.0);
}

TEST(AST, Eval)
{
    Root v = RBuilder::value(2.0);
    Root x = RBuilder::placeholder("x");

    Root f = RBuilder::add(v, x);

    Context ctx;
        ctx["x"] = v.get();

    EXPECT_DOUBLE_EQ(full_eval(ctx, f), 4.0);
}


