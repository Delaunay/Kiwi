#include "Expression.h"
#include "Visitor.h"

namespace kiwi {
VTABLEV(
void Function::visit(DynamicVisitor* v){
    v->function(this);
}
void FunctionCall::visit(DynamicVisitor* v){
    v->call(this);
}
void UnaryCall::visit(DynamicVisitor* v){
    v->unary_call(this);
}
void BinaryCall::visit(DynamicVisitor* v){
    v->binary_call(this);
}
void Value::visit(DynamicVisitor* v){
    v->value(this);
}
void Placeholder::visit(DynamicVisitor* v){
    v->placeholder(this);
}
)

std::size_t Call::args_size() const{
    switch(tag){
    case NodeTag::call1:
        return static_cast<const UnaryCall&>(*this).args_size();
    case NodeTag::call2:
        return static_cast<const BinaryCall&>(*this).args_size();
    case NodeTag::calln:
        return static_cast<const FunctionCall&>(*this).args_size();
    default:
        return 0;
    }
}

Expression* Call::arg(std::size_t index){
    switch(tag){
    case NodeTag::call1:
        return static_cast<UnaryCall&>(*this).arg(index);
    case NodeTag::call2:
        return static_cast<BinaryCall&>(*this).arg(index);
    case NodeTag::calln:
        return static_cast<FunctionCall&>(*this).arg(index);
    default:
        return nullptr;
    }
}

}
