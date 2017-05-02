#include "Expression.h"
#include "Visitor.h"

namespace kiwi {
VTABLEV(/*
void Add::visit(DynamicVisitor* v){
    v->add(this);
}*/
void UnaryFunction::visit(DynamicVisitor* v){
    v->unary(this);
}
void BinaryFunction::visit(DynamicVisitor* v){
    v->binary(this);
}
void NnaryFunction::visit(DynamicVisitor* v){
    v->nnary(this);
}

void Value::visit(DynamicVisitor* v){
    v->value(this);
}
void Placeholder::visit(DynamicVisitor* v){
    v->placeholder(this);
}
)


std::size_t Function::args_size() const {
    switch(tag){
    case NodeTag::unary:
        return static_cast<const UnaryFunction&>(*this).args_size();
    case NodeTag::binary:
        return static_cast<const BinaryFunction&>(*this).args_size();
    case NodeTag::nnary:
        return static_cast<const NnaryFunction&>(*this).args_size();
    default:
        return 0;
    }
}
Expression* Function::arg(std::size_t index) {
    switch(tag){
    case NodeTag::unary:
        return static_cast<UnaryFunction&>(*this).arg(index);
    case NodeTag::binary:
        return static_cast<BinaryFunction&>(*this).arg(index);
    case NodeTag::nnary:
        return static_cast<NnaryFunction&>(*this).arg(index);
    default:
        return nullptr;
    }
}

}
