#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <cassert>

#include "../Logging/Log.h"

namespace kiwi{

template<typename T>
T* check(T* ptr, const char* msg, int line){
    if (ptr == nullptr){
        log_error(SDL_GetError(), ", line: ", msg, ":", line);
        assert(ptr != nullptr);
    }
    return ptr;
}

template<typename T>
T check(T ptr, const char* msg, int line){
    if (ptr != 0){
        log_error("Value: ", ptr, ", ", SDL_GetError(), ", line: ", msg, ":", line);
    }
    return ptr;
}

#define HOME "../../fonts/"
#define CHECK(ptr) check(ptr, #ptr, __LINE__)
}
