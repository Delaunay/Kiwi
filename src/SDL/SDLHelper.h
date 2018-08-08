#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include <cassert>

#include <unordered_map>
#include <memory>

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

#define HOME "M:/projects/Kiwi/fonts/"
#define CHECK(ptr) check(ptr, #ptr, __FILE__, __LINE__)

struct SDL_Deleter {
	void operator()(SDL_Surface*  ptr) { if (ptr) SDL_FreeSurface(ptr); }
	void operator()(SDL_Texture*  ptr) { if (ptr) SDL_DestroyTexture(ptr); }
	void operator()(SDL_Renderer* ptr) { if (ptr) SDL_DestroyRenderer(ptr); }
	void operator()(SDL_Window*   ptr) { if (ptr) SDL_DestroyWindow(ptr); }
	void operator()(SDL_RWops*    ptr) { if (ptr) SDL_RWclose(ptr); }
	void operator()(TTF_Font*     ptr) { if (ptr) TTF_CloseFont(ptr); }
};

// We might want to switch to a different implementation later on
// --------------------------------------------------------------
template<typename K, typename V>
using Dict = std::unordered_map<K, V>;
using String = std::string;

using SurfaceRef = SDL_Surface * ;
using TextureRef = SDL_Texture * ;
using RendererRef = SDL_Renderer * ;
using WindowRef = SDL_Window * ;
using RWopsRef = SDL_RWops * ;
using FontRef = TTF_Font * ;

using SurfacePtr = std::unique_ptr<SDL_Surface, SDL_Deleter>;
using TexturePtr = std::unique_ptr<SDL_Texture, SDL_Deleter>;
using RendererPtr = std::unique_ptr<SDL_Renderer, SDL_Deleter>;
using WindowPtr = std::unique_ptr<SDL_Window, SDL_Deleter>;
using RWopsPtr = std::unique_ptr<SDL_RWops, SDL_Deleter>;
using FontPtr = std::unique_ptr<TTF_Font, SDL_Deleter>;

template<typename T, typename D>
T* get_ref(const std::unique_ptr<T, D>& ptr) {
	return ptr.get();
}

inline
void set_pixel(SDL_Surface* surface, i32 x, i32 y, SDL_Color color) {
	uchar* pixels = reinterpret_cast<uchar*>(surface->pixels);
	pixels[4 * i32(x + y * surface->w) + 0] = color.r;
	pixels[4 * i32(x + y * surface->w) + 1] = color.g;
	pixels[4 * i32(x + y * surface->w) + 2] = color.b;
	pixels[4 * i32(x + y * surface->w) + 3] = color.a;
}

inline
void render_surface(SDL_Renderer* renderer, i32 x, i32 y, SDL_Surface* surface, i32 w = -1, i32 h = -1) {
	SDL_Rect box{ x, y, w == -1 ? surface->w : w, h == -1 ? surface->h : h };
	TexturePtr hexT{ SDL_CreateTextureFromSurface(renderer, surface) };
	SDL_RenderCopy(renderer, hexT.get(), NULL, &box);
}

struct SurfaceLock {
	SurfaceLock(SDL_Surface* s) :
		_surface(s)
	{
		SDL_LockSurface(_surface);
	}

	~SurfaceLock() {
		SDL_UnlockSurface(_surface);
	}

private:
	SDL_Surface * _surface;
};
}
