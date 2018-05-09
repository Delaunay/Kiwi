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

#ADD_SUBDIRECTORY(glm)
#ADD_SUBDIRECTORY(gli)
#ADD_SUBDIRECTORY(eigen)
#ADD_SUBDIRECTORY(glfw)
#ADD_SUBDIRECTORY(glew)


# ------------------------------------------------------------------------------

# ADD_SUBDIRECTORY(SDL)
ADD_SUBDIRECTORY(json11)

