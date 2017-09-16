#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <cassert>

#include "../Logging/Log.h"

namespace kiwi{

template<typename T>
T* check(T* ptr, const char* msg, const char* file, int line){
    if (ptr == nullptr){
        log_error(SDL_GetError(), " ", file, ", line: ", line, ":", msg);
        assert(ptr != nullptr);
        SDL_ClearError();
    }
    return ptr;
}

// Error codes
template<typename T>
T check(T ptr, const char* msg, const char* file, int line){
    if (ptr != 0){
        log_error("Value: ", ptr, ", ", SDL_GetError(), file, ", line: ", line, ":", msg);
        SDL_ClearError();
    }
    return ptr;
}

#define HOME "../../fonts/"
#define CHECK(ptr) check(ptr, #ptr, __FILE__, __LINE__)
}
