#pragma once

#include <gtest/gtest.h>
#include <sstream>

#include "Parsing/Parsing.h"
#include "AST/TreeOperations.h"

using namespace kiwi;

TEST(Parser, AddParsing)
{
    std::string code = "(1 + 3) + x";

    StringBuffer reader(code);
    Parser       parser(reader);

    Root op = parser.parse();

    std::stringstream ss; print(ss, op);
    std::string name = ss.str();

    EXPECT_EQ(name, code);

    std::cout << "Parsed expr: " << name << std::endl;
}
