/*
#include "Expression.h"
#include "Value.h"
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

}
)
}*/
