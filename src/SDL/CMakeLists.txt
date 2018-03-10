INCLUDE(../../cmake/macros/GroupSources.cmake)
GroupSources(src/SDL)

INCLUDE_DIRECTORIES(../../dependencies/SDL/include)
INCLUDE_DIRECTORIES(../../dependencies/eigen)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${SDL2_TTF_INCLUDE_DIR})

SET(SDL_SRC
    Point.h
    String.h
    String.cpp
    RenderNode.h
    RenderTreeVisitor.h
    RenderEngine.h
    StyleManager.h
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
	ExpressionWorkspace.cpp
	ExpressionWorkspace.h
    SDLWrapper.h
    RenderingUtils.h
    RenderingUtils.cpp
)

#    SDLMain.cpp
#TARGET_LINK_LIBRARIES(sdl_rendering ast parsing ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY})

SET(${CXX_STANDARD_REQUIRED} ON)
ADD_LIBRARY(sdl_rendering ${SDL_SRC})
SET_PROPERTY(TARGET sdl_rendering PROPERTY CXX_STANDARD 17)

SET(${CXX_STANDARD_REQUIRED} ON)
ADD_EXECUTABLE(sdl_test_bed SDLTestBed.cpp)
TARGET_LINK_LIBRARIES(sdl_test_bed ast parsing sdl_rendering ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY})
SET_PROPERTY(TARGET sdl_test_bed PROPERTY CXX_STANDARD 17)

SET(${CXX_STANDARD_REQUIRED} ON)
ADD_EXECUTABLE(TIDE TIDE.cpp)
TARGET_LINK_LIBRARIES(TIDE ast parsing sdl_rendering ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY})
SET_PROPERTY(TARGET TIDE PROPERTY CXX_STANDARD 17)


