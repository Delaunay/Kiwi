FIND_PACKAGE(SFML 2 COMPONENTS system window graphics audio)

INCLUDE_DIRECTORIES(${SFML_INCLUDE_DIR})
INCLUDE_DIRECTORIES(../../dependencies/nuklear/)
INCLUDE_DIRECTORIES(../../dependencies/nuklear/demo)
INCLUDE_DIRECTORIES(../../dependencies/glad/include)

SET(FRONT_END_LIST
    ../../dependencies/nuklear/nuklear.h
    ../../dependencies/glad/src/glad.c
    ../../dependencies/glad/include/glad/glad.h

    nuklear_main.cpp
    Window.h
)

SET(${CXX_STANDARD_REQUIRED} ON)
ADD_EXECUTABLE(nuklear_main ${FRONT_END_LIST})
TARGET_LINK_LIBRARIES(nuklear_main parsing ast ${SFML_LIBRARIES})
SET_PROPERTY(TARGET nuklear_main PROPERTY CXX_STANDARD 14)
