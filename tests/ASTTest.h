#pragma once
#include <gtest/gtest.h>
#include <iostream>

#include "AST/Builder.h"
#include "AST/TreeOps.h"

#include "Format.h"

using namespace kiwi;

typedef generic::Root<Expression<LightExpression>> Root;
typedef Builder<LightExpression> RBuilder;

TEST(AST, Printing)
{
    Root v = RBuilder::value(2.0);
    Root w = RBuilder::value(3.0);
    Root x = RBuilder::placeholder("x");

    Root g = RBuilder::add(w, x);
    Root f = RBuilder::add(v, RBuilder::borrow(x));

    std::stringstream ss1;
    print_expr<LightExpression>(ss1, f);


    std::stringstream ss2;
    print_expr<LightExpression>(ss2, g);

    print(std::cout, "Result: ", ss1.str(), " and ", ss2.str()) << std::endl;
}


TEST(AST, Function)
{
    Root x    = RBuilder::placeholder("x");
    Root body = RBuilder::mult(x, RBuilder::borrow(x));
    Root sqr  = RBuilder::function("sqr", body);
         static_cast<LightAST::Function*>(sqr.get())->args.push_back("x");

    // add function to context
    Context<LightExpression> ctx = {
      {"sqr", sqr.get()}
    };

    // print function
    std::stringstream ss1;
    print_expr<LightExpression>(ss1, sqr);
    print(std::cout, "decl:\n", ss1.str()) << std::endl;

    // call function sqr(2)
    Root val  = RBuilder::value(3.0);
    Root sqr2 = RBuilder::call("sqr", {val.take_ownership()});

    std::stringstream ss2;
    print_expr<LightExpression>(ss2, sqr2);
    print(std::cout, "call: ", sqr2) << std::endl;

    EXPECT_DOUBLE_EQ(full_eval<LightExpression>(ctx, sqr2), 9.0);
}

TEST(AST, Eval)
{
    Root v = RBuilder::value(2.0);
    Root x = RBuilder::placeholder("x");

    Root f = RBuilder::add(v, x);

    Context<LightExpression> ctx;
        ctx["x"] = v.get();

    EXPECT_DOUBLE_EQ(full_eval<LightExpression>(ctx, f), 4.0);
}


