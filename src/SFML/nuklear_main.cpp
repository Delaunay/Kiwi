/*#include "Window.h"

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) < (b) ? (b) : (a))

#include "style.c"
#include "calculator.c"
#include "overview.c"
#include "node_editor.c"


class Application : public kiwi::Application<Application>
{
public:
    void draw_gui(){
        struct nk_context* ctx = _nuklear_ctx;
        struct nk_color background = _background_color;

        if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            enum {EASY, HARD};
            static int op = EASY;
            static int property = 20;

            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
                fprintf(stdout, "button pressed\n");

            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);

            if (nk_combo_begin_color(ctx, background, nk_vec2(nk_widget_width(ctx),400))) {
                nk_layout_row_dynamic(ctx, 120, 1);
                background = nk_color_picker(ctx, background, NK_RGBA);
                nk_layout_row_dynamic(ctx, 25, 1);
                background.r = (nk_byte)nk_propertyi(ctx, "#R:", 0, background.r, 255, 1,1);
                background.g = (nk_byte)nk_propertyi(ctx, "#G:", 0, background.g, 255, 1,1);
                background.b = (nk_byte)nk_propertyi(ctx, "#B:", 0, background.b, 255, 1,1);
                background.a = (nk_byte)nk_propertyi(ctx, "#A:", 0, background.a, 255, 1,1);
                nk_combo_end(ctx);
            }
        }
        nk_end(ctx);


        /* -------------- EXAMPLES ---------------- *
        //calculator(ctx);
        //overview(ctx);
        node_editor(ctx);
        /* ----------------------------------------- *
    }
};
*/



//#include "NuklearWindow.h"
/*
#include "SFMLWindow.h"

#include "../Logging/Log.h"

class App : public kiwi::SFMLWindow<App>{

};*/

//kiwi::NuklearWindow app;

/*
log_warn("Doing weird stuff");
log_trace("Doing weird stuff");
log_error("Doing weird stuff");
log_info("Doing weird stuff");
log_debug("Doing weird stuff");

for(int i = 0; i < 7; ++i)
    log_rec(i, "Doing weird stuff");

for(int i = 0; i < 8; ++i)
    log_rec(7 - i, "Doing weird stuff");



App app;

app.init();

app.run();

*/

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <iostream>

int main(){
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (ren == nullptr){
        SDL_DestroyWindow(win);
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    /*
    SDL_Event e;
    bool quit = false;
    while (!quit){
        while (SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                quit = true;
            }
        }
        //Render the scene
        SDL_RenderClear(ren);
        //renderTexture(image, ren, x, y);
        SDL_RenderPresent(ren);
    }*/


    SDL_Event e;
    bool quit = false;
    while(!quit && SDL_WaitEvent(&e)){
        if (e.type == SDL_QUIT){
            quit = true;
        }

        SDL_RenderClear(ren);
        //renderTexture(image, ren, x, y);
        SDL_RenderPresent(ren);
    }



    if (win == nullptr){
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    return 0;
}

