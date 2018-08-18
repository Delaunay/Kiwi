﻿#pragma once

#include "AST/LightAST.h"
#include <gtest/gtest.h>

//#define KIWI_DEBUG
#include "../src/Debug.h"

using namespace kiwi;

TEST(Value, val) {
    Value(2).dump(std::cout);
    std::cout << std::endl;
    Value(2.0).dump(std::cout);
    std::cout << std::endl;
    Value("hello").dump(std::cout);
    std::cout << std::endl;
}
