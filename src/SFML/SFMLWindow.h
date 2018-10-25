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

    void init(){ init(_window); }

    void init(sf::RenderWindow& screen){
        screen.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
                       "Kiwi", sf::Style::Default, _settings);
        //_window.setVerticalSyncEnabled(true);
        screen.setActive(true);

        //*
        if(!gladLoadGL()) {
            log_error("Failed to load OpenGL extensions!");
            std::abort();
        }//*/

        log_debug("glad loaded");

        ImGui::SFML::Init(screen);
        glViewport(0, 0, screen.getSize().x, screen.getSize().y);
    }

    void run()
    {
        log_trace("Loading Assets");
        start_clock();
        static_cast<Derived&>(*this).load_assets();

        log_trace("Asset loaded in: ", stop_clock(), "ms");

        while(_window.isOpen())
        {


            //log_trace("Events");
            static_cast<Derived&>(*this).process_events();
            static_cast<Derived&>(*this).process_network();

            //log_trace("Clear");
            static_cast<Derived&>(*this).clear(_window);
            //log_trace("Draw");
            static_cast<Derived&>(*this).draw(_window);
            //log_trace("Render");

            // start_clock();
            static_cast<Derived&>(*this).render(_window);
            // log_trace("render took: ", stop_clock());

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

    void draw_imgui(sf::RenderWindow& screen){
        ImGui::SFML::Update(screen, _deltaClock.restart());

        ImGui::Begin("Hello, world!");
          ImGui::Button("Look at this pretty button");
        ImGui::End();
    }

    void draw_rectangle(sf::RenderWindow& screen){
        sf::RectangleShape rect;
        rect.setFillColor(sf::Color(100, 100, 100));
        rect.setPosition(sf::Vector2f(100, 100));
        rect.setSize(sf::Vector2f(100, 100));
        screen.draw(rect);
    }

    void draw_fps(sf::RenderWindow& screen){
        static sf::Text fps_txt = [](){
            sf::Text txt;
            txt.setFont(default_font());
            txt.setPosition({WINDOW_WIDTH - 12 * 20, 0});
            txt.setFillColor({100, 100, 100});
            return txt;
        }();

        static int number_image = 0;
        number_image += 1;

        double value = stop_fps_clock();
        if (value > 10 * 1000){
            start_fps_clock();
            number_image = 0;
        }

        fps_txt.setString(to_string("FPS: ", number_image / (value / 1000)));
        screen.draw(fps_txt);
    }

    void draw_source(sf::RenderWindow& screen){
        _engine.render(sqr, {50, 50});
    }

    void draw(sf::RenderWindow& screen){
        draw_fps(screen);
        draw_rectangle(screen);

        draw_imgui(screen);

        //start_clock();
        draw_source(screen);
        //log_info("draw_source took: ", stop_clock(), "ms");
    }

    void render(sf::RenderWindow& screen){
        ImGui::SFML::Render(_window);
        screen.display();
    }

    void start_clock(){
        start_clock(start);
    }

    double stop_clock(){
        return stop_clock(start);
    }

    void start_fps_clock(){
        start_clock(start_fps);
    }

    double stop_fps_clock(){
        return stop_clock(start_fps);
    }

    static void start_clock(TimePoint& start){
        start = std::chrono::steady_clock::now();
    }

    static double stop_clock(TimePoint& start){
        typedef std::chrono::duration<long long, std::nano> nanoseconds;
        typedef std::chrono::duration<long long, std::micro> microseconds;
        typedef std::chrono::duration<double, std::milli> milliseconds;
        typedef std::chrono::duration<double> seconds;
        typedef std::chrono::duration<double, std::ratio<60> > minutes;
        typedef std::chrono::duration<double, std::ratio<3600> > hours;

        auto end = std::chrono::steady_clock::now();
        milliseconds dt = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return dt.count();
    }

protected:
    sf::Clock             _deltaClock;
    sf::ContextSettings   _settings;
    sf::RenderWindow      _window;
    RenderEngine          _engine;

    TimePoint start;
    TimePoint start_fps;
    generic::Root<RenderAST::Node>  sqr = RenderEngine::make_sqr();
};







}
