#include "../Logging/Log.h"

#include <memory>

#include <SDL.h>
#include <SDL_video.h>
#include <SDL_render.h>
#include <SDL_surface.h>

namespace kiwi{

    template<typename T>
    struct Point2{
        Point2(T x, T y):
            x(x), y(y)
        {}

        T x;
        T y;
    };

    typedef Point2<int>   Point2i;
    typedef Point2<float> Point2f;

    struct SQLBackend{
        typedef std::unique_ptr<SDL_Window  , SDL_DestroyWindow> Window;
        typedef std::unique_ptr<SDL_Renderer, SDL_DestroyRenderer> RenderEngine;

        SQLBackend(){
            if(SDL_init(SDL_INIT_VIDEO)){
                log_error("SDL was not able to load", SDL_GetError());
            }
        }

        Window create_window(const std::string& title, Point2i position, Point2i, size){
            Window* w = SDL_createWindow(title.c_str(), position.x, position.y, size.x, size.y);
            if (!w){
                log_error("SDL could not create screen", SDL_GetError());
            }
            return w;
        }

        RenderEngine create_render_engine(Window w){
            RenderEngine r = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (!r){
                log_error("SDL could not create render", SDL_GetError());
            }
            return r;
        }

        ~SQLBackend(){
            SQL_Quit();
        }
    };
}
