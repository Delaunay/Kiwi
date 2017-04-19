
SET(AST_SRC
    Expression.h
    Expression.cpp
    Builder.h
    Builder.cpp
    Visitor.h
    Printing.h
    Printing.cpp)

SET(${CXX_STANDARD_REQUIRED} ON)
ADD_LIBRARY(ast ${AST_SRC})
SET_PROPERTY(TARGET lexer PROPERTY CXX_STANDARD 14)
