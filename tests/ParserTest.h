#pragma once

#include <gtest/gtest.h>
#include <sstream>

#include "Parsing/Parsing.h"
#include "AST/TreeOperations.h"

using namespace kiwi;

TEST(Parser, AddParsing)
{
    std::string code = "((2 + 2) + (2 + 2)) + x";

    // first
    StringBuffer reader(code);
    Parser       parser(reader);
    Root op = parser.parse();
    std::stringstream ss; print(ss, op);
    std::string code_ret1 = ss.str();

//    std::cout << "Parsed expr: " << code_ret1 << std::endl;

    // second
    StringBuffer reader2(code_ret1);
    Parser       parser2(reader2);
    Root op2 = parser2.parse();
    std::stringstream ss2; print(ss2, op2);
    std::string code_ret2 = ss2.str();

    EXPECT_EQ(code_ret2, code_ret1);

//    std::cout << "Parsed expr: " << code_ret2 << std::endl;
}
