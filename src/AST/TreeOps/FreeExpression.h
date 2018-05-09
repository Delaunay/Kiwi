#pragma once

#include "../Root.h"
#include "../Expression.h"
#include "../Visitor.h"
#include "../../Logging/Log.h"

namespace kiwi {
    // Functions
    // ------------------------------------------------------------------------

    template<typename NodeTrait>
    void free(Expression<NodeTrait>* expr);


    // Implementation
    // ------------------------------------------------------------------------
    template<typename NodeTrait>
    class FreeMemory : public StaticVisitor<FreeMemory<NodeTrait>, NodeTrait, void> {
    public:
        FreeMemory() = default;

        static void run(Expression<NodeTrait>* expr) {
            if (expr == nullptr)
                return;

            FreeMemory<NodeTrait> eval;
            eval.traverse(expr);
            delete expr;
            return;
        }

        void function(Function<NodeTrait>* x) {
            traverse(x->body);
            delete x->body;
            return;
        }

        void general_call(Call<NodeTrait>& x) {
            for (int i = 0; i < x.args_size(); ++i) {
                traverse(x.arg(i));
                delete x.arg(i);
            }
            return;
        }

        void function_call(FunctionCall<NodeTrait>* x) {
            return general_call(*x);
        }

        void binary_call(BinaryCall<NodeTrait>* x) {
            return general_call(*x);
        }

        void unary_call(UnaryCall<NodeTrait>* x) {
            return general_call(*x);
        }

        void borrow(Borrow<NodeTrait>*) {
            // nothing to do. Borrow will be deleted by its parent
        }

        void value(Value<NodeTrait>* x) { return; }
        void placeholder(Placeholder<NodeTrait>* x) { return; }

        // TODO
        return_value arrow(Arrow<NodeTrait>* x) {
        }

        return_value type(Type<NodeTrait>* x) {
        }

        return_value builtin(Builtin<NodeTrait>* e) {
        }

        return_value error(ErrorNode<NodeTrait>* e) {
        }
    };


    template<typename NodeTrait>
    void free(Expression<NodeTrait>* expr) {
        return FreeMemory<NodeTrait>::run(expr);
    }
}