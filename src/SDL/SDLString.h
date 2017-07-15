#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>

#ifdef __linux__
#   include <SDL2/SDL_ttf.h>
#else
#   include <SDL_ttf.h>
#endif

#include "String.h"
#include "Point.h"

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
        switch(kind){
        case RenderingType::Solid  : return TTF_RenderUTF8_Solid(font, txt, color);
        case RenderingType::Blended: return TTF_RenderUTF8_Blended(font, txt, color);
        case RenderingType::Shaded : return TTF_RenderUTF8_Shaded(font, txt, color, {0, 0, 0});
        }
        return TTF_RenderUTF8_Solid(font, txt, color);
    }

    SDL_Surface* render_utf8(RenderingType kind, const String& txt, SDL_Font* font, SDL_Color color, SDL_Color color_bg){
        switch(kind){
        case RenderingType::Solid  : return TTF_RenderUTF8_Solid(font, txt, color);
        case RenderingType::Blended: return TTF_RenderUTF8_Blended(font, txt, color);
        case RenderingType::Shaded : return TTF_RenderUTF8_Shaded(font, txt, color, color_bg);
        }
        return TTF_RenderUTF8_Solid(font, txt, color);
    }

    SDLString(const char* txt, SDL_Font* font, SDL_Color color):
        txt(txt), surface(render_utf8(Blended, txt, font, color))
    {
        TTF_SizeText(font, txt, &width, &height);
    }

    ~SDLString(){
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    void render(SDL_Renderer *renderer, Point2i pos){
        SDL_Rect box;
        box.x = pos.x;
        box.y = pos.y;
        box.w = width;
        box.h = height;

        texture = CHECK(SDL_CreateTextureFromSurface(renderer, surface));
        CHECK(SDL_RenderCopy(renderer, texture, NULL, &box));
    }

private:
    const char  *txt     = nullptr;
    SDL_Surface *surface = nullptr;
    SDL_Texture *texture = nullptr;
    int width = 0;
    int height = 0;
};

}
