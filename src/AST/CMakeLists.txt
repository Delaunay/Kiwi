
SET(AST_SRC
    AST.h
    Root.h
    Expression.h
    Expression.cpp
    Value.h
    Builder.h
    Builder.cpp
    Visitor.h
    LightAST.h
    TreeOperations.h
    TreeOperations.cpp)

SET(${CXX_STANDARD_REQUIRED} ON)
ADD_LIBRARY(ast ${AST_SRC})
SET_PROPERTY(TARGET ast PROPERTY CXX_STANDARD 14)
