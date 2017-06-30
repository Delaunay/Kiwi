﻿#include <iostream>
#include <sstream>

#include "AST/Builder.h"
#include "AST/TreeOperations.h"

#include "Parsing/Parsing.h"

#include "Logging/Log.h"

using namespace kiwi;

int main()
{
    std::string code =
            "(1 / 3) + x";

    StringBuffer reader(code);

    Parser parser(reader);

    Parser::Root op = parser.parse();

    std::cout << "Parsed expr: ";
    print(std::cout, op);

    std::cout << std::endl;

    log_warn("A", 1, "B");

    /*
    Root v = Builder<>::value(2);
    Root w = Builder<>::value(3);
    Root x = Builder<>::placeholder("x");

    Root f = Builder<>::add(v, Builder<>::borrow(x));

    Root g = Builder<>::add(w, x);

    print(std::cout, f);
    std::cout << std::endl;

    print(std::cout, g);
    std::cout << std::endl;*/

    return 0;
}
