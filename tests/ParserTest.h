#pragma once

#include <gtest/gtest.h>
#include <sstream>

#include "AST/TreeOps.h"
#include "Parsing/Optional.h"
#include "Parsing/Parsing.h"

#include <iostream>
#define s(x) std::string(x)

using namespace kiwi;

TEST(Parser, AddParsing) {
    std::string code = "((2 + 2) + (2 + 2)) + x";

    // first
    StringBuffer reader(code);
    Parser parser(reader);
    Expression *op = parser.parse(0);

    std::stringstream ss;
    // print_expr<LightExpression>(ss, op);
    std::string code_ret1 = ss.str();

    //    std::cout << "Parsed expr: " << code_ret1 << std::endl;

    // second
    StringBuffer reader2(code_ret1);
    Parser parser2(reader2);
    Expression *op2 = parser2.parse(0);

    std::stringstream ss2;
    // print_expr<LightExpression>(ss2, op2);
    std::string code_ret2 = ss2.str();

    EXPECT_EQ(code_ret2, code_ret1);

    //    std::cout << "Parsed expr: " << code_ret2 << std::endl;
}

TEST(Parser, FunctionWithAnnotation) {
    std::string code = s("def inc(x: Double) -> Double:\n") + s("    return x + 1\n") + s("");

    StringBuffer reader(code);
    Parser parser(reader);

    //Tuple<String, Expression *> op = parser.parse_function(0);

    std::cout << std::string(80, '-') << std::endl;
    std::stringstream ss;
    // PrintStatement().visit_definition(std::get<1>(op), ss, 0);
    std::cout << std::string(80, '-') << std::endl;
    std::cout << ss.str() << std::endl;
}

TEST(Parser, FunctionWithoutAnnoation) {
    std::string code = s("def inc(x):\n") + s("    return x + 1\n") + s("");

    StringBuffer reader(code);
    Parser parser(reader);

    Tuple<String, Expression *> op = parser.parse_function(0);

    std::cout << std::string(80, '-') << std::endl;
    PrintExpression printer;
    std::stringstream ss;
    //printer.visit_expression(std::get<1>(data), ss, 0);
    //std::cout << std::get<0>(data) << ": " << std::endl << ss.str();
}

TEST(Parser, StructParsing) {
    std::string code = "struct Point(T1: Type, T2: Type):\n"
                       "    x: T1\n"
                       "    y: T2\n";

    StringBuffer reader(code);
    Parser parser(reader);

    Tuple<String, Expression *> data = parser.parse_record(0);

    PrintExpression printer;
    std::stringstream ss;
    printer.visit_expression(std::get<1>(data), ss, 0);
    std::cout << std::get<0>(data) << ": " << std::endl << ss.str();
}

TEST(Parser, UnionParsing) {
    std::string code = "union Point(T1: Type, T2: Type):\n"
                       "    x: T1\n"
                       "    y: T2\n";

    StringBuffer reader(code);
    Parser parser(reader);

    Tuple<String, Expression *> data = parser.parse_record(0);

    PrintExpression printer;
    std::stringstream ss;
    printer.visit_expression(std::get<1>(data), ss, 0);
    std::cout << std::get<0>(data) << ": " << std::endl << ss.str();
}

TEST(Parser, overall) {
    std::string code = ""
                       "def inc(x:)\n"
                       "    return x + 1\n"
                       "\n"
                       "struct Test(meta_type: Type)\n"
                       "    x: Int\n"
                       "    y: meta_type\n"
                       "\n"
                       "union Test(meta_type: Type)\n"
                       "    x: Int\n"
                       "    y: meta_type\n"
                       "\n"
                       "def my_fun(a: A, b: B) -> C:\n"
                       "    def wrap(x: X) -> Z:\n"
                       "        return x * a + b\n"
                       "    \n"
                       "    return wrap\n"
                       "\n";

    StringBuffer reader(code);
    Parser parser(reader);

    std::stringstream ss;
    std::cout << ss.str() << std::endl;
}
