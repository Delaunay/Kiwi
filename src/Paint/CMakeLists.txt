FIND_PACKAGE(SFML 2 COMPONENTS system window graphics audio)
FIND_PACKAGE(OpenGL)

INCLUDE_DIRECTORIES(${SFML_INCLUDE_DIR})
INCLUDE_DIRECTORIES(../../dependencies/nuklear/)
INCLUDE_DIRECTORIES(../../dependencies/nuklear/demo)
INCLUDE_DIRECTORIES(../../dependencies/glad/include)


SET(DEPENDENCIES
    ../../dependencies/nuklear/nuklear.h
    ../../dependencies/glad/src/glad.c
    ../../dependencies/glad/include/glad/glad.h
)

SET(FRONT_END_LIST
    ../../dependencies/nuklear/nuklear.h
    ../../dependencies/glad/src/glad.c
    ../../dependencies/glad/include/glad/glad.h

    nuklear_main.cpp
    SFMLWindow.h
    SFMLNuklear.h
    SFMLNuklear.cpp
    NuklearWindow.h

    Node.h
    Node.cpp

    NodeEditor.h
    NodeEditor.cpp
)

IF(WIN32)

ELSE()
    MESSAGE(STATUS "Linking with -ldl")
    SET(SYS_LIB -ldl)
ENDIF(WIN32)

SET(${CXX_STANDARD_REQUIRED} ON)
ADD_EXECUTABLE(nuklear_main ${FRONT_END_LIST})
TARGET_LINK_LIBRARIES(nuklear_main parsing ast
    ${SFML_LIBRARIES} ${OPENGL_LIBRARIES} ${SYS_LIB})
SET_PROPERTY(TARGET nuklear_main PROPERTY CXX_STANDARD 14)

SET(${CXX_STANDARD_REQUIRED} ON)
ADD_EXECUTABLE(nuklear_test main.cpp SFMLNuklear.cpp ../../dependencies/glad/src/glad.c)
TARGET_LINK_LIBRARIES(nuklear_test parsing ast
    ${SFML_LIBRARIES} ${SYS_LIB})
SET_PROPERTY(TARGET nuklear_test PROPERTY CXX_STANDARD 14)
