#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

// linux (memset/INT_MAX)
#include <cstring>
#include <climits>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SFML_GL3_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_sfml_gl3.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 712

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#define UNUSED(a) (void)a
#define LEN(a) (sizeof(a)/sizeof(a)[0])

#define FONT_FOLDER "../../dependencies/nuklear/extra_font/"

namespace kiwi{

template<typename Derived>
class Application{
public:
    Application():
        _settings(24, 8, 4, 3, 3),
        _window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
                    "Kiwi", sf::Style::Default, _settings)
    {
        _window.setVerticalSyncEnabled(true);
        _window.setActive(true);

        if(!gladLoadGL()) { /* Load OpenGL extensions */
            printf("Failed to load OpenGL extensions!\n");
        }

        glViewport(0, 0, _window.getSize().x, _window.getSize().y);

        _nuklear_ctx = nk_sfml_init(&_window);
        _background_color = nk_rgb(28,48,62);;
    }

    ~Application()
    {
        nk_sfml_shutdown();
    }

    void run()
    {
        static_cast<Derived&>(*this).load_fonts();

        while(_window.isOpen())
        {
            while(_window.pollEvent(_event)){
                nk_input_begin(_nuklear_ctx);
                static_cast<Derived&>(*this).process_events(_event);
                nk_sfml_handle_event(&_event);
                nk_input_end(_nuklear_ctx);
            }

            static_cast<Derived&>(*this).process_network();
            static_cast<Derived&>(*this).draw_gui();
            static_cast<Derived&>(*this).draw_scene();

            // Draw Stuff
            float bg[4];
            _window.setActive(true);
            nk_color_fv(bg, _background_color);
            glClear(GL_COLOR_BUFFER_BIT);
            glClearColor(bg[0], bg[1], bg[2], bg[3]);
            /* IMPORTANT: `nk_sfml_render` modifies some global OpenGL state
            * with blending, scissor, face culling and depth test and defaults everything
            * back into a default state. Make sure to either save and restore or
            * reset your own state after drawing rendering the UI. */
            nk_sfml_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
            _window.display();
        }
    }

    void draw_gui(){ }

    void process_events(sf::Event& e)
    {
        if (e.type == sf::Event::Closed)
            _window.close();

        else if(e.type == sf::Event::Resized)
            glViewport(0, 0, e.size.width, e.size.height);
    }

    void process_network()
    {}

    void draw_scene()
    {}

    void load_fonts(){
        nk_sfml_font_stash_begin(&_atlas);

        struct nk_font *droid = nk_font_atlas_add_from_file(
                    _atlas, FONT_FOLDER"DroidSans.ttf", 14, 0);
        /*
        struct nk_font *roboto = nk_font_atlas_add_from_file(
                    atlas, FONT_FOLDER"Roboto-Regular.ttf", 14, 0);
        struct nk_font *future = nk_font_atlas_add_from_file(
                    atlas, FONT_FOLDER"kenvector_future_thin.ttf", 13, 0);
        struct nk_font *clean = nk_font_atlas_add_from_file(
                    atlas, FONT_FOLDER"ProggyClean.ttf", 12, 0);
        struct nk_font *tiny = nk_font_atlas_add_from_file(
                    atlas, FONT_FOLDER"ProggyTiny.ttf", 10, 0);
        struct nk_font *cousine = nk_font_atlas_add_from_file(
                    atlas, FONT_FOLDER"Cousine-Regular.ttf", 13, 0); */

        nk_sfml_font_stash_end();
        nk_style_set_font(_nuklear_ctx, &droid->handle);
    }

protected:
    sf::ContextSettings   _settings;
    sf::Event             _event;
    sf::RenderWindow      _window;
    struct nk_context*    _nuklear_ctx;
    struct nk_color       _background_color;
    struct nk_font_atlas* _atlas;

};

}
