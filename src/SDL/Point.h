#pragma once

namespace kiwi{
template<typename T>
struct Point2{
    Point2(T x, T y):
        x(x), y(y)
    {}

    T x;
    T y;
};

typedef Point2<int>   Point2i;
typedef Point2<float> Point2f;
}
