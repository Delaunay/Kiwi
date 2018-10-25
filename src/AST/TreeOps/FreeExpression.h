#pragma once

#include "../../Logging/Log.h"
#include "../Expression.h"
#include "../Root.h"
#include "../Visitor.h"

namespace kiwi {
/*/ Functions
// ------------------------------------------------------------------------
void free(Expression *expr);

// Implementation
// ------------------------------------------------------------------------
class FreeMemory : public StaticVisitor<FreeMemory, void> {
  public:
    FreeMemory() = default;

    static void run(Expression *expr) {
        if(expr == nullptr)
            return;

        FreeMemory eval;
        eval.traverse(expr);
        delete expr;
        return;
    }

    void function(FunctionDeclaration *x) {
        traverse(x->body);
        delete x->body;
        return;
    }

    void general_call(Call &x) {
        for(int i = 0; i < x.args_size(); ++i) {
            traverse(x.arg(i));
            delete x.arg(i);
        }
        return;
    }

    void function_call(FunctionCall *x) { return general_call(*x); }

    void binary_call(BinaryCall *x) { return general_call(*x); }

    void unary_call(UnaryCall *x) { return general_call(*x); }

    void borrow(Borrow *) {
        // nothing to do. Borrow will be deleted by its parent
    }

    void value(Value *x) { return; }
    void placeholder(Placeholder *x) { return; }

    // TODO
    void arrow(FunctionType *x) {}

    void type(Type *x) {}

    void builtin(BuiltinType *e) {}

    void error(ErrorNode *e) {}
};

template <typename NodeTrait> void free(Expression *expr) { return FreeMemory::run(expr); }*/
} // namespace kiwi
