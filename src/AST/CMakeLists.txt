INCLUDE(../../cmake/macros/GroupSources.cmake)
GroupSources(src/AST)

SET(AST_SRC
    Tag.h
    Tag.cpp
    AST.h
    Root.h
    Statement.h
    Statement.cpp
    Definition.h
    Definition.cpp
    Expression.h
    Expression.cpp
    Type.h
    Type.cpp
    AbstractType.h
    Value.h
    Builder.h
    Builder.cpp
    Visitor.h
    LightAST.h
    StringDatabase.h
    StringDatabase.cpp
    Module.h
    Module.cpp
    TreeOps.h
    TreeOps/PrintExpression.h
    TreeOps/PrintExpression.cpp
    TreeOps/EvalExpression.h
    TreeOps/PartialEvalExpression.h
    TreeOps/FreeExpression.h
    TreeOps/JsonConversion.h
    TreeOps/CopyExpression.h
    TreeOps/Operators.h
    TreeOps/Operators.cpp
    ../Types.h)

SET(${CXX_STANDARD_REQUIRED} ON)
ADD_LIBRARY(ast ${AST_SRC})
TARGET_LINK_LIBRARIES(ast json11) 
SET_PROPERTY(TARGET ast PROPERTY CXX_STANDARD 17)
