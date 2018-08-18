#pragma once
#include <gtest/gtest.h>
#include <iostream>

#include "AST/Builder.h"
#include "AST/TreeOps.h"

#include "Format.h"

using namespace kiwi;

typedef Expression *Root;

TEST(AST, Printing) {
    Root v = Builder::value(2.0);
    Root w = Builder::value(3.0);
    Root x = Builder::placeholder("x");

    Root g = Builder::add(w, x);
    // Root f = Builder::add(v, Builder::borrow(x));

    std::stringstream ss1;
    // print_expr<LightExpression>(ss1, f);

    std::stringstream ss2;
    // print_expr<LightExpression>(ss2, g);

    // print(std::cout, "Result: ", ss1.str(), " and ", ss2.str()) << std::endl;
}

TEST(AST, Function) {
    Root x = Builder::placeholder("x");
    // Root body = Builder::mult(x, Builder::borrow(x));
    Definition *sqr = Builder::function("sqr", x);
    static_cast<FunctionDefinition *>(sqr)->args.push_back("x");

    // add function to context
    // Context<LightExpression> ctx = {{"sqr", sqr.get()}};

    // print function
    // std::stringstream ss1;
    // print_expr<LightExpression>(ss1, sqr);
    // print(std::cout, "decl:\n", ss1.str()) << std::endl;

    // call function sqr(2)
    Root val  = Builder::value(3.0);
    Root sqr2 = Builder::call(Builder::placeholder("sqr"), {val});

    std::stringstream ss2;
    // print_expr<LightExpression>(ss2, sqr2);
    // print(std::cout, "call: ", sqr2) << std::endl;

    // EXPECT_DOUBLE_EQ(full_eval<LightExpression>(ctx, sqr2), 9.0);
}

TEST(AST, Eval) {
    Root v = Builder::value(2.0);
    Root x = Builder::placeholder("x");

    Root f = Builder::add(v, x);

    Context ctx;
    ctx["x"] = v;

    // EXPECT_DOUBLE_EQ(full_eval<LightExpression>(ctx, f), 4.0);
}
