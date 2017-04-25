# FIND_PACKAGE(SFML)

INCLUDE_DIRECTORIES(${SFML_INCLUDE_DIR})
INCLUDE_DIRECTORIES(../../dependencies/nuklear/)

SET(FRONT_END_LIST
    ../../dependencies/nuklear/nuklear.h
    nuklear_main.cpp
)

SET(${CXX_STANDARD_REQUIRED} ON)
ADD_EXECUTABLE(nuklear_main ${FRONT_END_LIST})
TARGET_LINK_LIBRARIES(nuklear_main parsing ast ${SFML_LIBRARIES})
SET_PROPERTY(TARGET nuklear_main PROPERTY CXX_STANDARD 14)
