#include "Expression.h"
#include "Visitor.h"

namespace kiwi {
VTABLEV(
void Add::visit(DynamicVisitor* v){
    v->add(this);
}
void Value::visit(DynamicVisitor* v){
    v->value(this);
}
void Placeholder::visit(DynamicVisitor* v){
    v->placeholder(this);
}
)

}
