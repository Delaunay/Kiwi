##
##   If people don't build test don't add gtest
##
IF(BUILD_TESTING)
    # if gtest is installed to compile it again
    #IF(NOT ${GTEST_FOUND})
        ADD_SUBDIRECTORY(gtest)
        MESSAGE(STATUS "Compiling gtest from source")
    #ELSE()
    #    MESSAGE(STATUS "Using installed gtest library")
    #ENDIF()
ENDIF(BUILD_TESTING)

#ADD_SUBDIRECTORY(glfw)
#ADD_SUBDIRECTORY(glew)

# ==============================================================================
#           IMGUI
# ------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${SFML_INCLUDE_DIR})
INCLUDE_DIRECTORIES(imgui)

SET(IMGUI_SRC
    imgui/stb_truetype.h
    imgui/imgui.cpp
    imgui/imgui.h
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_internal.h
    imgui/stb_rect_pack.h
    imgui/stb_textedit.h
    # IMGUI-SFML
    imgui-sfml/imconfig-SFML.h
    imgui-sfml/imgui-SFML.cpp
    imgui-sfml/imgui-SFML.h
)

SET(IMGUI_HDS
    imgui/stb_truetype.h
    imgui/imconfig.h
    imgui/imgui.h
    imgui/imgui_internal.h
    imgui/stb_rect_pack.h
    imgui/stb_textedit.h
    # IMGUI-SFML
    imgui-sfml/imconfig-SFML.h
    imgui-sfml/imgui-SFML.h
    PARENT_SCOPE
)

SET(${CXX_STANDARD_REQUIRED} ON)
ADD_LIBRARY(imgui ${IMGUI_SRC})
SET_PROPERTY(TARGET imgui PROPERTY CXX_STANDARD 14)
# ------------------------------------------------------------------------------
