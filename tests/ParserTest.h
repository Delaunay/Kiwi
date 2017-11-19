#pragma once

#include <gtest/gtest.h>
#include <sstream>

#include "Parsing/Parsing.h"
#include "AST/TreeOperations.h"
#include "Parsing/Optional.h"

#include <iostream>
#define s(x) std::string(x)

using namespace kiwi;

TEST(Parser, AddParsing)
{
    std::string code = "((2 + 2) + (2 + 2)) + x";

    // first
    StringBuffer reader(code);
    Parser       parser(reader);
    Root op    = parser.parse(0);

    std::stringstream ss;
    print_expr(ss, op);
    std::string code_ret1 = ss.str();

//    std::cout << "Parsed expr: " << code_ret1 << std::endl;

    // second
    StringBuffer reader2(code_ret1);
    Parser       parser2(reader2);
    Root op2 = parser2.parse(0);

    std::stringstream ss2;
    print_expr(ss2, op2);
    std::string code_ret2 = ss2.str();

    EXPECT_EQ(code_ret2, code_ret1);

//    std::cout << "Parsed expr: " << code_ret2 << std::endl;
}

TEST(Parser, FunctionWithAnnotation)
{
	std::string code =
		s("def inc(x: Double) -> Double:\n") +
		s("    x + 1\n") +
		s("")
		;

	StringBuffer reader(code);
	Parser       parser(reader);

	Root op = parser.parse_function(0);

	std::stringstream ss;
	print_expr(ss, op);

	std::cout << ss.str() << std::endl;
}

TEST(Parser, FunctionWithoutAnnoation)
{
	std::string code =
		s("def inc(x):\n") +
		s("    x + 1\n") +
		s("")
		;

	StringBuffer reader(code);
	Parser       parser(reader);

	Root op = parser.parse_function(0);

	std::stringstream ss;
	print_expr(ss, op);

	std::cout << ss.str() << std::endl;
}




TEST(Optional, Some) {
	auto a = some<int>(10);

	EXPECT_EQ(a.is_defined(), true);
	EXPECT_EQ(a.is_empty(), false);
	EXPECT_EQ(a.get(), 10);

	a.foreach([](const int& i) -> void { std::cout << i << std::endl;  });

	int map_ret = a.map<int>([](const int& d) -> int { return d; }).get();
	EXPECT_EQ(map_ret, 10);

	int fold_ret = a.fold<int>(
		[]() { return 11;  }, 
		[](const int& a) { return a; }
	);

	EXPECT_EQ(fold_ret, 10);
}

TEST(Optional, None) {
	auto a = none<int>();

	EXPECT_EQ(a.is_defined(), false);
	EXPECT_EQ(a.is_empty(), true);
	EXPECT_THROW(a.get(), EmptyOption);

	a.foreach([](const int& i) -> void { std::cout << i << std::endl;  });

	Option<int> map_ret = a.map<int>([](const int& d) -> int { return d; });
	EXPECT_THROW(map_ret.get(), EmptyOption);

	int fold_ret = a.fold<int>(
		[]() { return 11;  }, 
		[](const int& a) { return a; }
	);

	EXPECT_EQ(fold_ret, 11);
}

TEST(Either, Right) {
	auto a = right<int, float>(10);

	EXPECT_EQ(a.is_right(), true);
	EXPECT_EQ(a.is_left(), false);

	EXPECT_EQ(a.right(), 10);
	EXPECT_THROW(a.left(), EitherError);

	bool fold_ret = a.fold<bool>(
		[](const int& a) -> bool { return true; },
		[](const float& b) -> bool { return false; }
	);

	EXPECT_EQ(fold_ret, true);
}

TEST(Either, Left) {
	auto a = left<int, float>(11.2);

	EXPECT_EQ(a.is_right(), false);
	EXPECT_EQ(a.is_left(), true);

	EXPECT_FLOAT_EQ(a.left(), 11.2);
	EXPECT_THROW(a.right(), EitherError);

	bool fold_ret = a.fold<bool>(
		[](const int& a) -> bool { return true; },
		[](const float& b) -> bool { return false; }
	);

	EXPECT_EQ(fold_ret, false);
}