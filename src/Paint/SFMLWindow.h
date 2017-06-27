#pragma once

//#include <SFML/OpenGL.hpp>
#include <glad/glad.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

#include "Fonts.h"

#define KIWI_DEBUG
#include "../Debug.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 712

#include "RenderEngine.h"

#include "../dependencies/imgui/imgui.h"
#include "../dependencies/imgui-sfml/imconfig-SFML.h"
#include "../dependencies/imgui-sfml/imgui-SFML.h"


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

        ImGui::SFML::Init(_window);
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

        /*
        std::string utf8_source = "...";
        std::basic_string<sf::Uint32> tmp;
        sf::Uint8::toUtf32(source.begin(), source.end(), std::back_inserter(tmp));
        sf::String final = tmp;*/

        /*
        sf::Text test;
        test.setFont(default_font());
        test.setString(L"Hello World");
        test.setFillColor(sf::Color(50, 100, 100));
        test.setOutlineColor(sf::Color(255, 255, 255));
        test.setOutlineThickness(1);
        screen.draw(test);

        sf::Text test2;
        test2.setPosition(sf::Vector2f(0, test.getLocalBounds().height));
        test2.setFont(default_font());
        test2.setString(L"Héllo World");
        test2.setFillColor(sf::Color(50, 100, 100));
        //test.setOutlineColor(sf::Color(100, 50, 100));
        //test.setOutlineThickness(1);
        screen.draw(test2);//*/


        ImGui::SFML::Update(_window, _deltaClock.restart());

        ImGui::Begin("Hello, world!");
          ImGui::Button("Look at this pretty button");
        ImGui::End();

        auto sqr = RenderEngine::make_sqr();
        RenderEngine::run(_window, sqr, {0, 0});
    }

    void render(sf::RenderWindow& screen){
        ImGui::SFML::Render(_window);
        screen.display();
    }

protected:
    sf::Clock             _deltaClock;
    sf::ContextSettings   _settings;
    sf::RenderWindow      _window;
};







}
