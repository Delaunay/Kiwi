#include <iostream>
#include <sstream>

#include "Lexer/Buffer.h"
#include "Lexer/Lexer.h"

#include "revision_data.h"

using namespace kiwi;

int main()
{
    std::cout << "\n"
                 "[0] Kiwi Interpreter \n"
                 "[0]   Compiler: " COMPILER_ID " " COMPILER_VERSION "\n"
                 "[0]     Branch: " _BRANCH "\n"
                 "[0]    Version: " _HASH "\n"
                 "[0]       Date: " _DATE "\n\n";

    std::string code =
            "def function(test:double, test): -> double\n"
            "    \"\"\" This is a docstring \"\"\"\n"
            "    return 1 + 1\n"
            "def function(test:int, test):\n"
            "    return 1 + 1\n";

    StringBuffer reader(code);

    Lexer lex(reader);

    // lex.print(std::cout);
    lex.debug_print(std::cout);

    std::cout << std::endl;

    return 0;
}
