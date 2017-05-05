#include "Expression.h"
#include "Visitor.h"

namespace kiwi {
VTABLEV(

#define X(name, object)\
    void object::visit(DynamicVisitor* v){\
        v->name(this);\
    }
    KIWI_AST_NODES
#undef X

void FunctionCall::visit(DynamicVisitor* v){
)

std::size_t Call::args_size() const{
    switch(tag){
    case NodeTag::unary_call:
        return static_cast<const UnaryCall&>(*this).args_size();
    case NodeTag::binary_call:
        return static_cast<const BinaryCall&>(*this).args_size();
    case NodeTag::function_call:
        return static_cast<const FunctionCall&>(*this).args_size();
    default:
        return 0;
    }
}

Expression* Call::arg(std::size_t index){
    switch(tag){
    case NodeTag::unary_call:
        return static_cast<UnaryCall&>(*this).arg(index);
    case NodeTag::binary_call:
        return static_cast<BinaryCall&>(*this).arg(index);
    case NodeTag::function_call:
        return static_cast<FunctionCall&>(*this).arg(index);
    default:
        return nullptr;
    }
}

}
