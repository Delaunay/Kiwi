#include "Expression.h"
#include "Visitor.h"

namespace kiwi {
VTABLEV(/*
void Add::visit(DynamicVisitor* v){
    v->add(this);
}*/
void Function::visit(DynamicVisitor* v){
    v->function(this);
}
void FunctionCcall::visit(DynamicVisitor* v){
    v->call(this);
}

void Value::visit(DynamicVisitor* v){
    v->value(this);
}
void Placeholder::visit(DynamicVisitor* v){
    v->placeholder(this);
}
)

}
