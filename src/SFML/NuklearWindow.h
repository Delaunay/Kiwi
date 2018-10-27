#pragma once

#include "SFMLWindow.h"
#include "SFMLNuklear.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#define FONT_FOLDER "../../dependencies/nuklear/extra_font/"

#include "Node.h"

namespace kiwi{

class NuklearWindow : public SFMLWindow<NuklearWindow> {
public:
    NuklearWindow()
    {}

    ~NuklearWindow()
    {}

    void load_assets(){
        _nuklear_ctx = nk_sfml_init(&_window);
        _background_color = nk_rgb(28,48,62);;

        nk_sfml_font_stash_begin(&_atlas);

        struct nk_font *droid = nk_font_atlas_add_from_file(
                    _atlas, FONT_FOLDER"DroidSans.ttf", 14, 0);

        nk_sfml_font_stash_end();
        nk_style_set_font(_nuklear_ctx, &droid->handle);
    }

    void tear_down(){
        nk_sfml_shutdown();
    }

    void process_events(){
        sf::Event event;

        nk_input_begin(_nuklear_ctx);
        while(_window.pollEvent(event)){
            process_event(event);
        }
        nk_input_end(_nuklear_ctx);
    }

    void process_event(sf::Event& e){
        if (e.type == sf::Event::Closed)
            _window.close();

        else if(e.type == sf::Event::Resized)
            glViewport(0, 0, e.size.width, e.size.height);

        nk_sfml_handle_event(&e);
    }

    void clear(sf::RenderWindow& screen){
        screen.setActive(true);
        //nk_color_fv(bg, _background_color);
        screen.clear(sf::Color(0, 0, 0));
        screen.setActive(true);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0, 0, 0, 0);
    }

    void draw(sf::RenderWindow& screen){
        sf::RectangleShape rect;
        rect.setFillColor(sf::Color(100, 100, 100));
        rect.setPosition(sf::Vector2f(100, 100));
        rect.setSize(sf::Vector2f(100, 100));

        screen.draw(rect);

        _node.draw(_nuklear_ctx);
    }

    void render(sf::RenderWindow& screen){
        /* IMPORTANT: `nk_sfml_render` modifies some global OpenGL state
        * with blending, scissor, face culling and depth test and defaults everything
        * back into a default state. Make sure to either save and restore or
        * reset your own state after drawing rendering the UI. */
        nk_sfml_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        screen.display();
    }

protected:
    Node                  _node;

    struct nk_context*    _nuklear_ctx;
    struct nk_color       _background_color;
    struct nk_font_atlas* _atlas;

};

}
