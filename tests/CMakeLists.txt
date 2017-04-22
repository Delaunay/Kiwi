
# tests need source header
INCLUDE_DIRECTORIES(../src)

#IF(NOT ${GTEST_FOUND})
#    SET(GTEST_LIBRARIES gtest)
#ENDIF()

## Add your test to CMAKE
## to run all tests run 'make test'
#MACRO(TEST_MACRO NAME LIBRARIES) # LIBRARIES
#    ADD_EXECUTABLE(${NAME}_test ${NAME}_test.cpp)
#    TARGET_LINK_LIBRARIES(${NAME}_test ${LIBRARIES} gtest -pthread)

#    ADD_TEST(NAME ${NAME}_test
#        COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${NAME}_test)
    
#    # gtest need to be compiled first
#    ADD_DEPENDENCIES(${NAME}_test gtest)

#    SET_PROPERTY(TARGET ${NAME}_test  PROPERTY CXX_STANDARD 11)
##ENDMACRO(TEST_MACRO)

## add test here
## file_name_test.cpp ==> CBTEST_MACRO(file_name)

SET(TEST_SRC
    ASTTest.h
    LexerTest.h
    ParserTest.h
)

ADD_EXECUTABLE(kiwi_test gtest_main.cpp ${TEST_SRC})
TARGET_LINK_LIBRARIES(kiwi_test ast parsing gtest -pthread)
SET_PROPERTY(TARGET kiwi_test PROPERTY CXX_STANDARD 14)


#TEST_MACRO(add      ${symdiff_libraries})
#TEST_MACRO(mult     ${symdiff_libraries})
#TEST_MACRO(inverse  ${symdiff_libraries})
#TEST_MACRO(opposite ${symdiff_libraries})
#TEST_MACRO(ln       ${symdiff_libraries})
#TEST_MACRO(exp      ${symdiff_libraries})
#TEST_MACRO(pow      ${symdiff_libraries})
#TEST_MACRO(equiv    ${symdiff_libraries})

