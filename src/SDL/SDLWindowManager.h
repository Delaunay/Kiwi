#pragma once

#ifdef __linux__
#   include <SDL2/SDL_ttf.h>
#else
#   include <SDL_ttf.h>
#endif

#include "SDLHelper.h"
#include "SDLWindow.h"
#include "String.h"

#include <unordered_map>
#include <list>

//#undef log_info
//#define log_info(...)


typedef TTF_Font SDL_Font;

namespace kiwi{
class WindowManager{
private:
    WindowManager(){
        log_trace("Window Manager is being initialized");
        CHECK(SDL_Init(SDL_INIT_VIDEO));
        CHECK(TTF_Init());

        //SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
        log_trace("Window Manager was initialized");
    }

public:
    ~WindowManager(){
        TTF_Quit();
        SDL_Quit();
    }

    static WindowManager& manager(){
        static WindowManager c;
        return c;
    }

    template<typename T>
    static T* new_window(const String& str, i32 x, i32 y, i32 w, i32 h, u32 flag = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE){
        return WindowManager::manager().make_window<T>(str, x, y, w, h, flag);
    }

    template<typename T>
    static T* new_window(const String& str, i32 w, i32 h, u32 flag = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE){
        return WindowManager::manager().
                make_window<T>(str, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, flag);
    }

    void window_exposed(u32 id){
        log_info("exposed ", id);
    }

    void window_mouse_enter(u32 id){
        log_info("mouse_enter ", id);

        if (mouse_focus())
            SDL_SetWindowTitle(smouse_focus(), "Not In Focus");

        set_mouse_focus(window(id));
        SDL_SetWindowTitle(smouse_focus(), get_state(id));
    }

    void window_mouse_leave(u32 id){
        log_info("mouse_leave ", id);

        if (mouse_focus())
            SDL_SetWindowTitle(smouse_focus(), get_state(id));
    }

    void window_gained_focus(u32 id){
        log_info("gained_focus ", id);

        if (keyboard_focus())
            SDL_SetWindowTitle(skeyboard_focus(), "Not In Focus");

		window(id)->set_focus(true);
        set_keyboard_focus(window(id));
        SDL_SetWindowTitle(skeyboard_focus(), get_state(id));
    }
    void window_lost_focus(u32 id){
        log_info("lost_focus ", id);

		window(id)->set_focus(true);
        SDL_SetWindowTitle(*window(id), "Not In Focus");
    }

    void set_keyboard_focus(SDLWindow* w) { _keyboard_focus_window = w;}
    void set_mouse_focus(SDLWindow* w)    { _mouse_focus_window = w;}

    SDLWindow* keyboard_focus(){ return _keyboard_focus_window; }
    SDLWindow* mouse_focus()   { return _mouse_focus_window; }
    SDLWindow* window(u32 id)  {
        //log_info("get id");
        return _windows[id];
    }

    SDL_Window* skeyboard_focus(){ return *_keyboard_focus_window; }
    SDL_Window* smouse_focus()   { return *_mouse_focus_window; }

    void window_shown(u32 id){
        log_info("shown ", id);
    }
    void window_hidden(u32 id){
        log_info("hidden ", id);
    }

    void window_moved(u32 id){
        log_info("moved ", id);
    }
    void window_resized(u32 id){
        log_info("resized ", id);
    }
    void window_size_changed(u32 id){
        log_info("size_changed ", id);
    }

    void window_maximized(u32 id){
        log_info("maximized ", id);
    }
    void window_minimized(u32 id){
        log_info("minimized ", id);
    }
    void window_restored(u32 id){
        log_info("restored ", id);
    }

    void window_close(u32 id){
        log_info("close ", id);

        // you can close with keyboard shortcut
        if (keyboard_focus() || mouse_focus()){

            SDLWindow* target = window(id);
            SDL_HideWindow(*target);

            //*
            delete target;
            _windows.erase(id);//*/
            _visible_windows -= 1;


            if (keyboard_focus() == target){
                _keyboard_focus_window = nullptr;
            }
            if (mouse_focus() == target){
                _mouse_focus_window = nullptr;
            }
            if (_visible_windows == 0)
                _is_running = false;
        }

        log_info("close - done");
    }
    void window_take_focus(u32 id){
        log_info("take_focus ", id);
    }


    void handle_window_event(SDL_WindowEvent event){
        u32 id = event.windowID;
        SDLWindow* w = _windows[id];

        if (!w){
            return;
        }

        w->handle_window(event);

        switch(event.event){
            case SDL_WINDOWEVENT_EXPOSED     : return window_exposed(id);

            case SDL_WINDOWEVENT_ENTER       : return window_mouse_enter(id);
            case SDL_WINDOWEVENT_LEAVE       : return window_mouse_leave(id);

            case SDL_WINDOWEVENT_FOCUS_GAINED: return window_gained_focus(id);
            case SDL_WINDOWEVENT_FOCUS_LOST  : return window_lost_focus(id);

            case SDL_WINDOWEVENT_SHOWN       : return window_shown(id);
            case SDL_WINDOWEVENT_HIDDEN      : return window_hidden(id);

            case SDL_WINDOWEVENT_MOVED       : return window_moved(id);

            case SDL_WINDOWEVENT_RESIZED     : return window_resized(id);
            case SDL_WINDOWEVENT_SIZE_CHANGED: return window_size_changed(id);

            case SDL_WINDOWEVENT_MAXIMIZED   : return window_maximized(id);
            case SDL_WINDOWEVENT_MINIMIZED   : return window_minimized(id);
            case SDL_WINDOWEVENT_RESTORED    : return window_restored(id);

            case SDL_WINDOWEVENT_CLOSE       : return window_close(id);
            case SDL_WINDOWEVENT_TAKE_FOCUS  : return window_take_focus(id);
            //case SDL_WINDOWEVENT_HIT_TEST:
        }
        log_info("unhandled window event");
    }

    const char* get_state(u32 id){
        if (window(id) == mouse_focus() && window(id) == keyboard_focus())
            return "Mouse & Keyboard";
        if (window(id) == mouse_focus())
            return "Mouse";
        if (window(id) == keyboard_focus())
            return "Keyboard";
        return "Not in Focus";
    }


    void run(){
        _is_running = true;

        SDL_Event event;
        while(is_running() && SDL_WaitEvent(&event)){
            switch (event.type){
            case SDL_WINDOWEVENT:{
                handle_window_event(event.window);
                break;
            }

            case SDL_MOUSEMOTION:
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEWHEEL:{
                if (mouse_focus()){
                    mouse_focus()->handle_event(event);
                    mouse_focus()->render();
                }
                break;
            }
            case SDL_DROPFILE:
            case SDL_DROPTEXT:
            case SDL_DROPBEGIN:
            case SDL_DROPCOMPLETE:{
                if (mouse_focus()){
                    mouse_focus()->handle_event(event);
                    mouse_focus()->render();
                }
                SDL_free(event.drop.file);
                break;
            }
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            case SDL_TEXTEDITING:
            case SDL_TEXTINPUT:{
                if (keyboard_focus()){
                    keyboard_focus()->handle_event(event);
                    keyboard_focus()->render();
                    break;
                }
                break;
            }
            case SDL_QUIT:
                log_info("SDL_QUIT");
                break;

            default:
                log_info("unhandled event");
                break;

            }
        }
    }

    bool is_running(){
        return _is_running;
    }

    void stop(){
        _is_running = false;
    }


    template<typename T>
    T* push_back(SDL_Window* win){
        // _windows.push_back(new T(win));
        T* e = new T(win);
        _windows[SDL_GetWindowID(win)] = e;
        return e;
    }

private:
    template<typename T>
    T* make_window(const String& str, i32 x, i32 y, i32 w, i32 h, u32 flag){
        SDL_Window* sdlwin = CHECK(SDL_CreateWindow(str.c_str(), x, y, w, h, flag));
        T* win = push_back<T>(sdlwin);
        log_debug("Window ID: ", win->id());
        _visible_windows += 1;
        return win;
    }

    bool _is_running = false;
    std::unordered_map<int, SDLWindow*> _windows;
    SDLWindow* _mouse_focus_window = nullptr;
    SDLWindow* _keyboard_focus_window = nullptr;
    u32 _visible_windows = 0;
};

}
