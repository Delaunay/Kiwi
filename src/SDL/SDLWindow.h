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
protected:
    friend class WindowManager;

    SDLWindow(SDL_Window* handle):
        _handle(handle)
    {
        log_debug("Window(", id(), ") was created");
        _renderer = CHECK(SDL_CreateRenderer(_handle, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
        _font = CHECK(TTF_OpenFont(HOME "DejaVuSansMonoPowerline.ttf", 50));
    }

public:
    SDLWindow() = default;

    SDLWindow(const SDLWindow& w):
        _handle(w._handle), _renderer(w._renderer), _font(w._font)
    {}

    ~SDLWindow(){
    }

    bool operator== (const SDLWindow& w){
        return _handle == w._handle;
    }

    void handle_event(const SDL_Event& event){
        //log_info("event handling");
        switch (event.type){
        case SDL_MOUSEWHEEL:      return handle_mouse_wheel(event.wheel);

        case SDL_WINDOWEVENT:     return handle_window(event.window);

        case SDL_MOUSEMOTION:     return handle_mouse_motion(event.motion);

        case SDL_MOUSEBUTTONDOWN: return handle_mouse_button_down(event.button);
        case SDL_MOUSEBUTTONUP:   return handle_mouse_button_up(event.button);

        case SDL_KEYDOWN:         return handle_keyboard_down(event.key);
        case SDL_KEYUP:           return handle_keyboard_up(event.key);

        case SDL_DROPFILE:        return handle_drop_file(event.drop);
        case SDL_DROPTEXT:        return handle_drop_text(event.drop);
        case SDL_DROPBEGIN:       return handle_drop_begin(event.drop);
        case SDL_DROPCOMPLETE:
            handle_drop_complete(event.drop);
            SDL_free(event.drop.file);
            break;

        // SDL_SetTextInputRect
        // SDL_StartTextInput
        // SDL_StopTextInput
        case SDL_TEXTEDITING: return handle_text_editing(event.edit); // Char by char
        case SDL_TEXTINPUT: return handle_text_input(event.text); // Grouped input
        }


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
        log_debug("Nothing to do ", id());
        /*
        if (event.window.event == SDL_WINDOWEVENT_CLOSE){
            log_debug("Window(", id(), ") Close event was received");
            SDL_HideWindow(_handle);
        }*/
    }

    virtual void draw(SDL_Renderer *renderer){
        CHECK(SDL_RenderDrawLine(renderer, 0, 0, 100, 100));
    }

    virtual void render(){
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
        return _handle != nullptr;
    }

    virtual void handle_mouse_button_up(SDL_MouseButtonEvent event) {}
    virtual void handle_mouse_button_down(SDL_MouseButtonEvent event) {}

    virtual void handle_keyboard_up(SDL_KeyboardEvent event) {}
    virtual void handle_keyboard_down(SDL_KeyboardEvent event) {}

    virtual void handle_drop_file(SDL_DropEvent event) {}
    virtual void handle_drop_text(SDL_DropEvent event) {}
    virtual void handle_drop_begin(SDL_DropEvent event) {}
    virtual void handle_drop_complete(SDL_DropEvent event) {}

    virtual void handle_audio_device(SDL_AudioDeviceEvent event) {}

    virtual void handle_quit(SDL_QuitEvent event) {}

    virtual void handle_mouse_motion(SDL_MouseMotionEvent event) {}

    virtual void handle_mouse_wheel(SDL_MouseWheelEvent event) {}

    virtual void handle_text_editing(SDL_TextEditingEvent event) {}

    virtual void handle_text_input(SDL_TextInputEvent event) {}

    virtual void handle_window(SDL_WindowEvent wevent){
        switch(wevent.event){
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

    SDL_Renderer* renderer() { return _renderer; }

protected:
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

#define UTF8_CODE_POINT(x, y, z)
UTF8_CODE_POINT(E0A0, 	EE 82 A0, Branch)
UTF8_CODE_POINT(E0A1, 	EE 82 A1, Line number)
UTF8_CODE_POINT(E0A2, 	EE 82 A2, Padlock (read-only))
UTF8_CODE_POINT(E0A3, 	EE 82 A3, Column number)
UTF8_CODE_POINT(E0B0, 	EE 82 B9, Right angle solid)
UTF8_CODE_POINT(E0B1, 	EE 82 B1, Right angle line)
UTF8_CODE_POINT(E0B2, 	EE 82 B2, Left angle solid)
UTF8_CODE_POINT(E0B3, 	EE 82 B3, Left angle line)
UTF8_CODE_POINT(E0B8, 	EE 82 B8, Bottom-left angle solid)
UTF8_CODE_POINT(E0B9, 	EE 82 B9, Bottom-left angle line)
UTF8_CODE_POINT(E0BA, 	EE 82 BA, Bottom-right angle solid)
UTF8_CODE_POINT(E0BB, 	EE 82 BB, Bottom-right angle line)
UTF8_CODE_POINT(E0BC, 	EE 82 BC, Top-left angle solid)
UTF8_CODE_POINT(E0BD, 	EE 82 BD, Top-left angle line)
UTF8_CODE_POINT(E0BE, 	EE 82 BE, Top-right angle solid)
UTF8_CODE_POINT(E0BF, 	EE 82 BF, Top-right angle line)

