#pragma once

#include "Types.h"
#include <string>
#include <sstream>
#include <cmath>

namespace kiwi
{

inline std::ostream& print(std::ostream& out){
    return out;
}

template<typename T, typename... Args>
std::ostream& print(std::ostream& out, T value, const Args& ... args){
    out << value;
    return print(out, args...);
}

template<typename T, typename... Args>
std::string to_string(T value, const Args& ... args){
    std::stringstream ss;
    print(ss, value, args...);
    return ss.str();
}

inline
std::string align_left(const std::string& name, uint8 col){
    if (name.size() > col)
        return name.substr(0, col);

    if (name.size() == col)
        return name;

    return name + std::string(col - name.size(), ' ');
}

inline
std::string align_right(const std::string& name, uint8 col){
    if (name.size() > col)
        return name.substr(0, col);

    if (name.size() == col)
        return name;

    return std::string(col - name.size(), ' ') + name;
}

inline
std::string align_center(const std::string& name, uint8 col){
    if (name.size() > col)
        return name.substr(0, col);

    if (name.size() == col)
        return name;

    uint32 n = uint32(col - name.size());
    n = (n + n % 2)/ 2;

    return std::string(n, ' ') + name + std::string(n, ' ');
}

template<typename T>
std::string to_string_padded(T nb, int32 b){

    int32 n = int32(std::log10(nb)) + 1;

    if (nb == 0)
        n = 1;

    n = b - n;
    if (n > 0)
        return std::string(n, ' ') + std::to_string(nb);
    return std::to_string(nb);
}

inline
std::string to_string_padded(float32 nb, int32 b){
    int32 n = int32(std::log10(nb)) + 2;
    n = b - n;
    if (n > 0)
        return std::string(n, ' ') + std::to_string(nb);
    return std::to_string(nb);
}

inline
std::string to_string_padded(float64 nb, int32 b){
    int32 n = int32(std::log10(nb)) + 2;
    n = b - n;
    if (n > 0)
        return std::string(n, ' ') + std::to_string(nb);
    return std::to_string(nb);
}

}
