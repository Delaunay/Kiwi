﻿# required for C++11 features
CMAKE_MINIMUM_REQUIRED(VERSION 3.8)

# project name + language used
PROJECT(kiwi LANGUAGES CXX CUDA)

# Compiler Stuff
# ====================================
# required for gtest which uses /MD
if (MSVC)
    # Zlib bug
    ADD_DEFINITIONS(-DZLIB_WINAPI)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
    ADD_DEFINITIONS(-DNOMINMAX)
    ADD_COMPILE_OPTIONS(/MD)  #  -l409
ELSE(MSVC)
    ADD_COMPILE_OPTIONS(-fno-rtti)
    MESSAGE(STATUS "Compilling without rtti -fno-rtti")
ENDIF(MSVC)

IF(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    MESSAGE(STATUS "Building Debug Version")
ELSE()
    MESSAGE(STATUS "Building Release Version")
    #if (MSVC)
    #    ADD_COMPILE_OPTIONS(/O2)
    #ELSE(MSVC)
    #    ADD_COMPILE_OPTIONS(-O2)
    #ENDIF(MSVC)
ENDIF()

# specify where compiled files must go
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# add cmake FindXXX script to find pre-compiled libraries if any
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

SET(${CXX_STANDARD_REQUIRED} ON)

# Project's Options
# ====================================

OPTION(BUILD_TESTING        "Enable tests"       ON)
OPTION(USE_LLVM_IR          "Enable LLVM-IR"     OFF)
OPTION(BUILD_EXAMPLES       "Build examples"     OFF)
OPTION(BUILD_BENCHMARK      "Build Benchmarks"   OFF)
OPTION(BUILD_DOCUMENTATION  "Build docs"         OFF)
OPTION(BUILD_DOXYGEN        "Build Doxygen docs" OFF)
OPTION(BUILD_SPHINX         "Build Sphinx docs"  OFF)
OPTION(BUILD_UI             "Build UI"           OFF)

# Binary/pre-compiled Dependencies
# ====================================

#FIND_PACKAGE(Git)
FIND_PACKAGE(gtest QUIET)

#IF(${BUILD_UI})
#    # Add SFML cmake script
#    LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/dependencies/SFML/cmake/Modules")
#    # Where SFML should be
#    SET(SFML_ROOT "${CMAKE_SOURCE_DIR}/dependencies/SFML/")
#    # Find SFML
#    FIND_PACKAGE(SFML 2 COMPONENTS graphics window system)
#ENDIF()


#IF(${USE_LLVM_IR})
#    # you need that variable to be present
#    # SET LLVM_DIR C:/Program Files/LLVM/share/llvm/cmake
#    FIND_PACKAGE(LLVM REQUIRED CONFIG)

#    IF(${LLVM_FOUND})
#        MESSAGE(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
#        MESSAGE(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

#        INCLUDE_DIRECTORIES(${LLVM_INCLUDE_DIRS})
#        ADD_DEFINITIONS(${LLVM_DEFINITIONS})

#        llvm_map_components_to_libnames(LLVM_LIBS
#            Support
#            Core
#            nativecodegen)
#            #ExecutionEngine
#            #Interpreter
#            #MC
#    ENDIF()
#ENDIF()

# Configuration
# FIND_PACKAGE(SFML 2 COMPONENTS system window graphics audio)
# FIND_PACKAGE(OpenGL)

SET(SDL2_PATH ./dependencies/SDL)
SET(SDL2_TTF_PATH ./dependencies/SDL_ttf)

#SET(SDL2_BUILDING_LIBRARY ON)
FIND_PACKAGE(SDL2 REQUIRED)
FIND_PACKAGE(SDL2_ttf)

# Find revision ID and hash of the sourcetree
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})
INCLUDE(cmake/genrev.cmake)

# Subdirectories
# ====================================

# Dependencies to be compiled
ADD_SUBDIRECTORY(dependencies)


# How to compile main lib
# =========================
ADD_SUBDIRECTORY(src)

# Compile tests ?
IF(BUILD_TESTING)
    ENABLE_TESTING()

    MESSAGE(STATUS "Building tests")
    ADD_SUBDIRECTORY(tests)
ENDIF(BUILD_TESTING)

IF(BUILD_EXAMPLES)
    MESSAGE(STATUS "Building examples")
    ADD_SUBDIRECTORY(examples)
ENDIF(BUILD_EXAMPLES)

IF(BUILD_BENCHMARK)
    MESSAGE(STATUS "Building Benchmarks")
    ADD_SUBDIRECTORY(bench)
ENDIF(BUILD_BENCHMARK)


# cmake shortcut
# How to make those not run in make all
ADD_CUSTOM_TARGET(compile-db
    cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ${CMAKE_SOURCE_DIR})

# should I pass tests too ?
ADD_CUSTOM_TARGET(clang-tidy
    clang-tidy -p= ${CMAKE_BINARY_DIR} -header-filter=.*
        ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/src/*.h)
