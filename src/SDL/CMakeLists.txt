INCLUDE_DIRECTORIES(../../dependencies/SDL/include)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${SDL2_TTF_INCLUDE_DIR})

SET(SDL_SRC
    Point.h
    String.h
    String.cpp
    SDLEvent.h
    SDLEvent.cpp
    SDLString.h
    SDLString.cpp
    SDLHelper.h
    SDLHelper.cpp
    SDLWindowManager.h
    SDLWindowManager.cpp
    SDLWindow.h
    SDLWindow.cpp
    SDLMain.cpp)

MESSAGE(STATUS ${SDL2_LIBRARY})

SET(${CXX_STANDARD_REQUIRED} ON)
ADD_EXECUTABLE(sdl_main ${SDL_SRC})
TARGET_LINK_LIBRARIES(sdl_main ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY})
SET_PROPERTY(TARGET sdl_main PROPERTY CXX_STANDARD 14)
