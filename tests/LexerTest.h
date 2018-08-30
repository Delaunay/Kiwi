#pragma once
#include "Parsing/Lexer.h"
#include <gtest/gtest.h>

using namespace kiwi;

TEST(Lexer, TokenDump) {
    std::string code = "def function(test:double, test): -> double\n"
                       "    \"\"\" This is a docstring \"\"\"\n"
                       "    return 1 + 1\n"
                       "def function(test:int, test):\n"
                       "    return 1 + 1;\n";

    StringBuffer reader(code);

    Lexer lex(reader);

    // lex.print(std::cout);
    lex.debug_print(std::cout);

    std::cout << std::endl;
}
