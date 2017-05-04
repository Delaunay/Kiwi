#pragma once

#include <gtest/gtest.h>
#include "AST/Value.h"

//#define KIWI_DEBUG
#include "../src/Debug.h"

using namespace kiwi;

TEST(Value, val)
{
    Value(2)  .dump(std::cout);
    std::cout << std::endl;
    Value(2.0).dump(std::cout);
    std::cout << std::endl;
    Value("hello").dump(std::cout);
    std::cout << std::endl;

}
