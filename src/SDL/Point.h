#pragma once

#include <Eigen/Dense>

#include <SDL.h>

namespace kiwi {

template <typename T> struct Point2 {
    Point2(T x = 0, T y = 0) : x(x), y(y) {}

    T x;
    T y;
};

typedef Point2<int> Point2i;
typedef Point2<float> Point2f;
typedef Point2f Point;

template <typename T> Point2<T> operator+(Point2<T> a, Point2<T> b) {
    return Point2<T>(a.x + b.x, a.y + b.y);
}

template <typename T> bool operator==(Point2<T> a, Point2<T> b) {
    return (a.x == b.x && a.y == b.y);
}

template <typename T> bool operator!=(Point2<T> a, Point2<T> b) { return !(a == b); }

template <typename T> struct Point4 {
    Point4(T x, T y, T w, T h) : x(x), y(y), width(w), height(h) {}

    T x;
    T y;
    T width;
    T height;
};

template <typename T> struct Color4 {
    Color4(T r, T g, T b, T a = 0) : r(r), g(g), b(b), a(a) {}

    T r;
    T g;
    T b;
    T a;

    operator SDL_Color() { return SDL_Color{r, g, b, a}; }
};

template <typename T> Point4<T> operator+(Point4<T> a, Point4<T> b) {
    return Point4<T>(a.x + b.x, a.y + b.y, a.width + b.width, a.height + b.height);
}

typedef Point4<int> Point4i;
typedef Point4<float> Point4f;
typedef Color4<Uint8> Color;

// typedef Point4i       Rectangle;

// typedef Eigen::Matrix<float, 4, 1> Color;
// typedef Eigen::Matrix<float, 2, 1> Point;
// typedef Eigen::Matrix<float, 4, 1> Rectangle;
} // namespace kiwi
