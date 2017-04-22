#pragma once

#include <gtest/gtest.h>

#include "Parsing/Parsing.h"
#include "AST/TreeOperations.h"

using namespace kiwi;

TEST(Parser, AddParsing)
{
    std::string code =
            "1 + x + 3";

    StringBuffer reader(code);

    Parser parser(reader);

    Root op = parser.parse();

    std::cout << "Parsed expr: ";
    print(std::cout, op);

    std::cout << std::endl;
}
