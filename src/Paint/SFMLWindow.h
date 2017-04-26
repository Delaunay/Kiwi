#pragma once

#include <glad/glad.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
//#include <SFML/OpenGL.hpp>

#define KIWI_DEBUG
#include "../Debug.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 712


namespace kiwi{

template<typename Derived>
class SFMLWindow{
public:
    SFMLWindow():
        _settings(24, 8, 4, 3, 3)
    {}

    ~SFMLWindow()
    {}

    void init(){
        _window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
                       "Kiwi", sf::Style::Default, _settings);
        _window.setVerticalSyncEnabled(true);
        _window.setActive(true);

        //*
        if(!gladLoadGL()) {
            printd("Failed to load OpenGL extensions!");
            std::abort();
        }//*/
        printd("glad loaded");

        glViewport(0, 0, _window.getSize().x, _window.getSize().y);
    }

    void run()
    {
        static_cast<Derived&>(*this).load_assets();

        while(_window.isOpen())
        {
            static_cast<Derived&>(*this).process_events();
            static_cast<Derived&>(*this).process_network();

            static_cast<Derived&>(*this).clear(_window);
            static_cast<Derived&>(*this).draw(_window);
            static_cast<Derived&>(*this).render(_window);

            // don't use 99% of CPU
            sf::sleep(sf::microseconds(1));
        }

        static_cast<Derived&>(*this).tear_down();
    }

    void process_events(){
        sf::Event e;

        while(_window.pollEvent(e)){
            static_cast<Derived&>(*this).process_event(e);
        }
    }

    void process_event(sf::Event& e){
        if (e.type == sf::Event::Closed)
            _window.close();
    }

    void process_network(){
    }

    void load_assets(){
    }

    void tear_down(){
    }

    void clear(sf::RenderWindow& screen){
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
    }

    void render(sf::RenderWindow& screen){
        screen.display();
    }

protected:
    sf::ContextSettings   _settings;
    sf::RenderWindow      _window;
};







}
