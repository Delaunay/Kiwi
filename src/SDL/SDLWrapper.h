#include <SDL.h>
#include <SDL_ttf.h>
#include <memory>

namespace sdl {

    struct SDL_Deleter {
        void operator()(SDL_Surface*  ptr) { if (ptr) SDL_FreeSurface(ptr); }
        void operator()(SDL_Texture*  ptr) { if (ptr) SDL_DestroyTexture(ptr); }
        void operator()(SDL_Renderer* ptr) { if (ptr) SDL_DestroyRenderer(ptr); }
        void operator()(SDL_Window*   ptr) { if (ptr) SDL_DestroyWindow(ptr); }
        void operator()(SDL_RWops*    ptr) { if (ptr) SDL_RWclose(ptr); }
        void operator()(TTF_Font*     ptr) { if (ptr) TTF_CloseFont(ptr); }
    };

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


}