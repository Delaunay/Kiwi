#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <SDL_video.h>
#include <SDL_render.h>
#include <SDL_surface.h>

#include "SDLHelper.h"
#include "SDLEvent.h"
#include "../Types.h"

#include "Point.h"

#include "SDLString.h"

namespace kiwi{

class SDLWindow{
private:
    friend class WindowManager;

    SDLWindow(SDL_Window* handle):
        _handle(handle)
    {
        log_debug("Window(", id(), ") was created");
        _renderer = CHECK(SDL_CreateRenderer(_handle, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
        _font = CHECK(TTF_OpenFont(HOME "DejaVuSansMono.ttf", 50));
    }

public:
    SDLWindow() = default;

    SDLWindow(const SDLWindow& w):
        _handle(w._handle), _renderer(w._renderer), _font(w._font)
    {}

    ~SDLWindow(){}

    bool operator== (const SDLWindow& w){
        return _handle == w._handle;
    }

    void handle_event(const SDL_Event& event){
        if (event.type == SDL_MOUSEMOTION)
            return;

        log_debug(to_string(event));

        if(event.type == SDL_WINDOWEVENT && event.window.windowID == id()){
            switch(event.window.event){
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                log_debug("SDL_WINDOWEVENT_SIZE_CHANGED ", id());
                SDL_RenderPresent(_renderer);
                log_debug("resized");
                break;

                case SDL_WINDOWEVENT_EXPOSED:
                log_debug("SDL_WINDOWEVENT_EXPOSED ", id());
                SDL_RenderPresent(_renderer);
                break;

                case SDL_WINDOWEVENT_CLOSE:
                log_debug("SDL_WINDOWEVENT_CLOSE ", id());
                SDL_HideWindow(_handle);
                break;
            }
        }
        //log_debug("Nothing to do ", id());
        /*
        if (event.window.event == SDL_WINDOWEVENT_CLOSE){
            log_debug("Window(", id(), ") Close event was received");
            SDL_HideWindow(_handle);
        }*/
    }

    void draw(SDL_Renderer *renderer){
        CHECK(SDL_RenderDrawLine(renderer, 0, 0, 100, 100));
    }

    void render(){
        // Clear Black
//        log_debug("1 ", id());
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
//        log_debug("2 ", id());
        SDL_RenderClear(_renderer);
//        log_debug("3 ", id());
        SDL_SetRenderDrawColor(_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
//        log_debug("4 ", id());
        draw(_renderer);
//        log_debug("5 ", id());
        SDL_RenderPresent(_renderer);
//        log_debug("6 ", id());
//        log_debug("--------------------------------------");
    }

    i32 id(){ return SDL_GetWindowID(_handle); }

    operator SDL_Window const*() const{
        return _handle;
    }

    operator SDL_Window*(){
        return _handle;
    }

    operator bool() const{
        return _handle;
    }

private:
    mutable SDL_Window* _handle = nullptr;

    SDL_Renderer *_renderer = nullptr;
    SDL_Color     _bg_color = {255, 255, 255};
    SDL_Font*     _font     = nullptr;
    SDL_Surface*  _txt      = nullptr;
    SDL_Texture*  _message  = nullptr;
};

}

/*
        log_debug("Starting init >>>>>>>>>>>>");
        _renderer = CHECK(SDL_CreateRenderer(_window, -1,
                                       SDL_RENDERER_ACCELERATED
                                     | SDL_RENDERER_PRESENTVSYNC));

        SDL_Color fg_color = {155, 255, 255};


        txt = CHECK(TTF_RenderUTF8_Blended(font, " \xE2\x86\x92 ", fg_color));
        message = CHECK(SDL_CreateTextureFromSurface(_renderer, txt));
        log_debug("init end <<<<<<<<<<<<<<<<<");
*/
