
SET(LEXER_SRC
    Parsing.h
    Stream.h
    Buffer.h
    Buffer.cpp
    Tokens.cpp
    Tokens.h
    Lexer.h
    Lexer.cpp
    Parser.h
    Parser.cpp
    Expected.h
    Optional.h
)

SET(${CXX_STANDARD_REQUIRED} ON)
ADD_LIBRARY(parsing ${LEXER_SRC})
SET_PROPERTY(TARGET parsing PROPERTY CXX_STANDARD 17)
