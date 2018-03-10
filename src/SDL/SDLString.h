#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>

#ifdef __linux__
#   include <SDL2/SDL_ttf.h>
#else
#   include <SDL_ttf.h>
#endif

#include "SDLHelper.h"
#include "String.h"
#include "Point.h"

#undef log_trace
#define log_trace(...)

namespace kiwi{

typedef TTF_Font SDL_Font;

enum RenderingType{
    Solid,
    Blended,
    Shaded
};

struct SDLString{
public:
    SDL_Surface* render_utf8(RenderingType kind, const String& txt, SDL_Font* font, SDL_Color color){
        CHECK(font);
        switch(kind){
        case RenderingType::Solid  : return CHECK(TTF_RenderUTF8_Solid(font, txt.c_str(), color));
        case RenderingType::Blended: return CHECK(TTF_RenderUTF8_Blended(font, txt.c_str(), color));
        case RenderingType::Shaded : return CHECK(TTF_RenderUTF8_Shaded(font, txt.c_str(), color, {0, 0, 0}));
        }
        return CHECK(TTF_RenderUTF8_Solid(font, txt.c_str(), color));
    }

    SDL_Surface* render_utf8(RenderingType kind, const String& txt, SDL_Font* font, SDL_Color color, SDL_Color color_bg){
        CHECK(font);
        switch(kind){
        case RenderingType::Solid  : return CHECK(TTF_RenderUTF8_Solid(font, txt.c_str(), color));
        case RenderingType::Blended: return CHECK(TTF_RenderUTF8_Blended(font, txt.c_str(), color));
        case RenderingType::Shaded : return CHECK(TTF_RenderUTF8_Shaded(font, txt.c_str(), color, color_bg));
        }
        return CHECK(TTF_RenderUTF8_Solid(font, txt.c_str(), color));
    }

    static SDL_Font* default_font() {
        static SDL_Font* font = CHECK(TTF_OpenFont(HOME "DejaVuSansMonoPowerline.ttf", 24));
        return font;
    }

    static SDL_Color default_color() {
        return SDL_Color{255, 255, 255};
    }

    template<typename T>
    SDLString(const T& txt, SDL_Color color = default_color(), SDL_Font* font = default_font()):
        txt(txt), surface(render_utf8(Blended, string(), font, color))
    {
        log_trace("Creating String, ");
        TTF_SizeText(font, string(), &_width, &_height);
    }

    SDLString(SDLString&& str):
        txt(std::move(str.txt)), surface(str.surface),
        _width(str._width), _height(str._height)
    {
        SDL_DestroyTexture(str.texture);
        str.surface = nullptr;
        str.texture = nullptr;
    }

    SDLString(const SDLString &str) = delete;

    ~SDLString(){
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    void render(SDL_Renderer *renderer, Point2i pos) const{
        SDL_Rect box;
        box.x = pos.x;
        box.y = pos.y;
        box.w = _width;
        box.h = _height;

        log_trace("Creating texture ", string());
        SDL_DestroyTexture(texture);
        texture = CHECK(SDL_CreateTextureFromSurface(renderer, surface));
        log_trace("Texture created");
        CHECK(SDL_RenderCopy(renderer, texture, NULL, &box));
    }

    int width()   const { return _width;  }
    int height()  const { return _height; }

    const char* string()  const { return txt.c_str(); }

private:
    std::string txt;
    SDL_Surface *surface = nullptr;
    mutable SDL_Texture *texture = nullptr;
    int _width = 0;
    int _height = 0;
};

}
