
SET(LEXER_SRC
    Stream.h
    Buffer.h
    Buffer.cpp
    Lexer.h
    Lexer.cpp
    Tokens.cpp
    Tokens.h)

SET(${CXX_STANDARD_REQUIRED} ON)
ADD_LIBRARY(lexer ${LEXER_SRC})
SET_PROPERTY(TARGET lexer PROPERTY CXX_STANDARD 14)
