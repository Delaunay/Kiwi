

namespace kiwi{

template<typename Backend>
class Window{

};

template<typename Backend>
class Font{

};

template<typename Backend>
class String{

};

template<typename Backend>
class Rectangle{

};


template<typename Backend>
class RenderingEngine{

};

template<typename Backend>
class Event{

};


template<typename Derived>
class EventDrivenRendering{
    void run(){
        SDL_Event* event;
        bool running = true;

        while(running && SDL_WaitEvent(event)){
            if (e.type == SDL_QUIT){
                running = false;
            }
        }

        if (running){
            log_error("SDL_WaitEvent returned 0, an error occured", SDL_GetError());
        }
    }
};

template<typename Derived>
class RealTimeRendering{
    void run(){
        SDL_Event* event;
        bool running = true;

        while(running){
            while(SDL_PollEvent(&event)){
                if (e.type == SDL_QUIT){
                    running = false;
                }
            }
        }
    }
};


}
