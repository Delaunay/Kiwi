#pragma once

#include <chrono>

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



template<int N, typename fun>
void timeit(const char* msg, fun f){
    auto start = std::chrono::steady_clock::now();
    for(int i = 0; i < N; ++i){
        f();
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> dt = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    printf("%s", msg);
    printf("took: %f sec\n", dt.count());
}


namespace kiwi{

template<typename Derived>
class SFMLWindow{
public:
    typedef std::chrono::time_point<std::chrono::steady_clock> TimePoint;

    SFMLWindow():
        _settings(24, 8, 4, 3, 3), _engine(_window)
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
            log_error("Failed to load OpenGL extensions!");
            std::abort();
        }//*/

        log_debug("glad loaded");

        ImGui::SFML::Init(_window);
        glViewport(0, 0, _window.getSize().x, _window.getSize().y);
    }

    void run()
    {
        log_trace("Loading Assets");
        start_clock();
        static_cast<Derived&>(*this).load_assets();

        log_trace("Asset loaded in: ", stop_clock(), "s");

        while(_window.isOpen())
        {
            number_image += 1;

            log_trace("Events");
            static_cast<Derived&>(*this).process_events();
            static_cast<Derived&>(*this).process_network();

            log_trace("Clear");
            static_cast<Derived&>(*this).clear(_window);
            log_trace("Draw");
            static_cast<Derived&>(*this).draw(_window);
            log_trace("Render");
            static_cast<Derived&>(*this).render(_window);

            // don't use 99% of CPU
            sf::sleep(sf::microseconds(1));
        }

        log_trace("Tear Down");
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


        log_trace("IMGUI update");
        ImGui::SFML::Update(_window, _deltaClock.restart());

        ImGui::Begin("Hello, world!");
          ImGui::Button("Look at this pretty button");
        ImGui::End();

        log_trace("Rendering...");
        _engine.render(sqr, {10, 10});

        log_trace(std::to_string(_engine.bounding_boxes.size()));
        log_trace("Done");
    }

    void render(sf::RenderWindow& screen){
        ImGui::SFML::Render(_window);
        screen.display();
    }

    void start_clock(){
        start = std::chrono::steady_clock::now();
    }

    double stop_clock(){
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> dt = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return dt.count();
    }

protected:
    sf::Clock             _deltaClock;
    sf::ContextSettings   _settings;
    sf::RenderWindow      _window;
    RenderEngine          _engine;

    TimePoint start;
    int number_image = 0;

    generic::Root<RenderAST::Node>  sqr = RenderEngine::make_sqr();
};







}
