#include <iostream>
#include <sstream>

#include "AST/Builder.h"
#include "AST/TreeOps.h"

#include "Parsing/Parsing.h"

#include "Logging/Log.h"

using namespace kiwi;

int main()
{
    std::string code =
            "(1 / 3) + x";

    StringBuffer reader(code);

    Parser parser(reader);

    Parser::Root op = parser.parse(0);

    std::stringstream ss; 
    print_expr<LightExpression>(ss, op);

    print(std::cout, "Parsed expr: ", ss.str()) << std::endl;

    log_warn("A", 1, "B");

    std::cout<< to_string("A", 1, "B") << std::endl;

    log_cwarn(0, "Message 1");
    log_cwarn(1, "Message 2");
    log_cwarn(2, "Message 3");
    log_cwarn(3, "Message 4");
    log_cwarn(2, "Message 5");


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
