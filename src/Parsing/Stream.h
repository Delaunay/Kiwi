#pragma once
#include "../Types.h"

namespace kiwi{
template<typename Derived, typename TokenType>
class Stream{
    void consume(){
        return static_cast<Derived const&>(*this).consume();
    }

    TokenType peek(){
        return static_cast<Derived const&>(*this).peek();
    }

    uint32 line(){
        return static_cast<Derived const&>(*this).line();
    }

    uint32 col(){
        return static_cast<Derived const&>(*this).col();
    }
};
}

