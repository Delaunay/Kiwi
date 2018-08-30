#ifndef KIWI_OPTIONAL_TEST_HEADER_
#define KIWI_OPTIONAL_TEST_HEADER_

#include <gtest/gtest.h>

#include "Parsing/Optional.h"

TEST(Optional, Some) {
    auto a = some<int>(10);

    EXPECT_EQ(a.is_defined(), true);
    EXPECT_EQ(a.is_empty(), false);
    EXPECT_EQ(a.get(), 10);

    a.foreach([](const int &i) -> void { std::cout << i << std::endl; });

    int map_ret = a.map<int>([](const int &d) -> int { return d; }).get();
    EXPECT_EQ(map_ret, 10);

    int fold_ret = a.fold<int>([]() { return 11; }, [](const int &a) { return a; });

    EXPECT_EQ(fold_ret, 10);
}

TEST(Optional, None) {
    auto a = none<int>();

    EXPECT_EQ(a.is_defined(), false);
    EXPECT_EQ(a.is_empty(), true);
    EXPECT_THROW(a.get(), EmptyOptionException);

    a.foreach([](const int &i) -> void { std::cout << i << std::endl; });

    Option<int> map_ret = a.map<int>([](const int &d) -> int { return d; });
    // EXPECT_THROW(map_ret.get(), EmptyOptionException);

    int fold_ret = a.fold<int>([]() { return 11; }, [](const int &a) { return a; });

    EXPECT_EQ(fold_ret, 11);
}

TEST(Either, Right) {
    auto a = right<int, f32>(10);

    EXPECT_EQ(a.is_right(), true);
    EXPECT_EQ(a.is_left(), false);

    EXPECT_EQ(a.right(), 10);
    EXPECT_THROW(a.left(), EitherError);

    bool fold_ret = a.fold<bool>([](const int &a) -> bool { return true; },
                                 [](const float &b) -> bool { return false; });

    EXPECT_EQ(fold_ret, true);
}

TEST(Either, Left) {
    auto a = left<int, f32>(11.2f);

    EXPECT_EQ(a.is_right(), false);
    EXPECT_EQ(a.is_left(), true);

    EXPECT_FLOAT_EQ(a.left(), 11.2f);
    EXPECT_THROW(a.right(), EitherError);

    bool fold_ret = a.fold<bool>([](const int &a) -> bool { return true; },
                                 [](const float &b) -> bool { return false; });

    EXPECT_EQ(fold_ret, false);
}

#endif
