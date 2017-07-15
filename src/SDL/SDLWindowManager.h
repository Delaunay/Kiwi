#pragma once

#ifdef __linux__
#   include <SDL2/SDL_ttf.h>
#else
#   include <SDL_ttf.h>
#endif

#include "SDLHelper.h"
#include "SDLWindow.h"
#include "String.h"


typedef TTF_Font SDL_Font;

namespace kiwi{
class WindowManager{
private:
    WindowManager(){
        CHECK(SDL_Init(SDL_INIT_VIDEO));
        CHECK(TTF_Init());

        //SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
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

    static SDLWindow new_window(const String& str, i32 x, i32 y, i32 w, i32 h, u32 flag = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE){
        return WindowManager::manager().make_window(str, x, y, w, h, flag);
    }

    static SDLWindow new_window(const String& str, i32 w, i32 h, u32 flag = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE){
        return WindowManager::manager().
                make_window(str, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, flag);
    }

    void window_exposed(u32 id){
        log_info("exposed ", id);
    }

    void window_mouse_enter(u32 id){
        log_info("mouse_enter ", id);
        SDL_SetWindowTitle(_mouse_focus_window, "Not In Focus");
        _mouse_focus_window = _windows[id];
        SDL_SetWindowTitle(_mouse_focus_window, get_state(id));
    }

    void window_mouse_leave(u32 id){
        log_info("mouse_leave ", id);
        SDL_SetWindowTitle(_mouse_focus_window, get_state(id));
    }

    void window_gained_focus(u32 id){
        log_info("gained_focus ", id);
        SDL_SetWindowTitle(_keyboard_focus_window, "Not In Focus");
        _keyboard_focus_window = _windows[id];
        SDL_SetWindowTitle(_keyboard_focus_window, get_state(id));
    }
    void window_lost_focus(u32 id){
        log_info("lost_focus ", id);
        SDL_SetWindowTitle(_keyboard_focus_window, get_state(id));
    }

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
        SDL_HideWindow(_keyboard_focus_window);
        _visible_windows -= 1;

        if (_visible_windows == 0)
            _is_running = false;
    }
    void window_take_focus(u32 id){
        log_info("take_focus ", id);
    }

    void handle_window_event(SDL_WindowEvent event){
        u32 id = event.windowID - 1;
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
    }

    const char* get_state(u32 id){
        if (_windows[id] == _mouse_focus_window && _windows[id] == _keyboard_focus_window)
            return "Mouse & Keyboard";
        if (_windows[id] == _mouse_focus_window)
            return "Mouse";
        if (_windows[id] == _keyboard_focus_window)
            return "Keyboard";
        return "Not in Focus";
    }


    void run(){
        _is_running = true;

        SDL_Event event;
        while(is_running() && SDL_WaitEvent(&event)){
            switch (event.type){
            case SDL_WINDOWEVENT:
                handle_window_event(event.window);
                break;

            case SDL_MOUSEMOTION:
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEWHEEL:
                _mouse_focus_window.handle_event(event);
                _mouse_focus_window.render();
                break;

            case SDL_DROPFILE:
            case SDL_DROPTEXT:
            case SDL_DROPBEGIN:
            case SDL_DROPCOMPLETE:
                SDL_free(event.drop.file);
                break;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                _keyboard_focus_window.handle_event(event);
                _keyboard_focus_window.render();
                break;

            // SDL_SetTextInputRect
            // SDL_StartTextInput
            // SDL_StopTextInput
            // Char by char
            case SDL_TEXTEDITING:

            // Grouped input
            case SDL_TEXTINPUT:
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

    SDLWindow last(){
        return _windows[_windows.size() - 1];
    }

    SDLWindow emplace_back(SDL_Window* win){
        _windows.push_back(win);
        return last();
    }

private:
    SDLWindow make_window(const String& str, i32 x, i32 y, i32 w, i32 h, u32 flag){
        SDL_Window* sdlwin = CHECK(SDL_CreateWindow(str, x, y, w, h, flag));
        SDLWindow win = emplace_back(sdlwin);
        log_debug("Window ID: ", win.id());
        _visible_windows += 1;
        return win;
    }

    bool _is_running = false;
    std::vector<SDLWindow> _windows;
    SDLWindow _mouse_focus_window;
    SDLWindow _keyboard_focus_window;
    u32 _visible_windows = 0;
};

}
