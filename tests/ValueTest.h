#pragma once

#include <gtest/gtest.h>
#include "AST/LightAST.h"

//#define KIWI_DEBUG
#include "../src/Debug.h"

using namespace kiwi;

TEST(Value, val)
{
    LightAST::Value(2)  .dump(std::cout);
    std::cout << std::endl;
    LightAST::Value(2.0).dump(std::cout);
    std::cout << std::endl;
    LightAST::Value("hello").dump(std::cout);
    std::cout << std::endl;

}
