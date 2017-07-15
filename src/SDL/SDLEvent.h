#pragma once
#include <SDL.h>

namespace kiwi {

    #define SHOW_EVENT(name) case name: return #name;
    inline
    const char* to_string(const SDL_Event& event){

        switch(event.type){
            SHOW_EVENT(SDL_WINDOWEVENT);
            SHOW_EVENT(SDL_SYSWMEVENT);
            SHOW_EVENT(SDL_KEYDOWN);
            SHOW_EVENT(SDL_KEYUP);
            SHOW_EVENT(SDL_TEXTEDITING);
            SHOW_EVENT(SDL_TEXTINPUT);
            SHOW_EVENT(SDL_KEYMAPCHANGED);
            SHOW_EVENT(SDL_MOUSEMOTION);
            SHOW_EVENT(SDL_MOUSEBUTTONDOWN);
            SHOW_EVENT(SDL_MOUSEBUTTONUP);
            SHOW_EVENT(SDL_MOUSEWHEEL);
            SHOW_EVENT(SDL_CLIPBOARDUPDATE);
            SHOW_EVENT(SDL_DROPFILE);
            SHOW_EVENT(SDL_DROPTEXT);
            SHOW_EVENT(SDL_DROPBEGIN);
            SHOW_EVENT(SDL_DROPCOMPLETE);
            SHOW_EVENT(SDL_RENDER_TARGETS_RESET);
            SHOW_EVENT(SDL_RENDER_DEVICE_RESET);
        default: return "NOT HANDLED EXCEPTION";
        }
    }
}
