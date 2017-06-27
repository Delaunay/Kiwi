#pragma once

#include <SFML/Graphics.hpp>

#include "../AST/Visitor.h"

#include "RenderNode.h"

namespace kiwi{

template<typename Visitor, typename RetType, typename ...Args>
class RenderTreeVisitor: public generic::StaticVisitor<Visitor, RenderAST, RetType, Args...>{

    /* To be implemented:
    RetType function(Function* x, Args...);
    RetType new_line(RetType pos, Args...);
    RetType indent(RetType pos, int idt, Args...);
    RetType function_call(FunctionCall* x, Args...);
    RetType unary_call(UnaryCall* x, Args...);
    RetType binary_call(BinaryCall* x, Args...);
    RetType borrow(Borrow* b, Args...);
    RetType arrow(Arrow* x, Args...);
    RetType type(Type* x, Args...);
    RetType builtin(Builtin* e, Args...);
    RetType error(ErrorNode* e, Args...);
    RetType value(Value* x, Args...);

    RetType placeholder(Placeholder* x); //*/
};
}
