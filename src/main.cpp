#include <iostream>
#include <sstream>

#include "AST/Builder.h"
#include "AST/TreeOperations.h"


using namespace kiwi;

int main()
{
    Root v = Builder::value(2);
    Root w = Builder::value(3);
    Root x = Builder::placeholder("x");

    Root f = Builder::add(v, x);

    Root g = Builder::add(w, x);

    print(std::cout, f);
    std::cout << std::endl;

    print(std::cout, g);
    std::cout << std::endl;

    return 0;
}
