#pragma once

#include "../Root.h"
#include "../Expression.h"
#include "../Visitor.h"
#include "../../Logging/Log.h"

namespace kiwi {
    // Functions
    // ------------------------------------------------------------------------






    // Implementation
    // ------------------------------------------------------------------------
    template<typename NodeTrait>
    class PartialEval : public StaticVisitor<PartialEval<NodeTrait>, NodeTrait, generic::Root<Expression<NodeTrait>>> {
    public:
        typedef typename generic::Root<Expression<NodeTrait>> Root;
        typedef typename generic::DummyRoot<Expression<NodeTrait>> DummyRoot;

        PartialEval(const Context<NodeTrait>& ctx) :
            ctx(ctx)
        {}

        static Root run(const Context<NodeTrait>& ctx, Expression<NodeTrait>* expr) {
            PartialEval eval(ctx);
            return eval.traverse(expr);
        }

        Expression<NodeTrait>* function(Function<NodeTrait>* x) {
            return x;
        }

        Expression<NodeTrait>* function_call(FunctionCall<NodeTrait>* x) {
            // we need to build a new context with the arguments

            // lookup the implementation

            // eval body with new context

            // return result
            return x;
        }

        Expression<NodeTrait>* unary_call(UnaryCall* x) {
            Expression<NodeTrait>* expr = traverse(x->expr);

            if (expr->tag == NodeTag::value) {
                Value<NodeTrait>* vexpr = static_cast<Value*>(expr);
                return Builder<DummyRoot, Tree>::value(
                    unary_operator(x->name)(vexpr->as<f64>()));
            }

            return Builder<DummyRoot, Tree>::unary_call(x->name, expr);
        }

        Expression<NodeTrait>* binary_call(BinaryCall<NodeTrait>* x) {
            Expression<NodeTrait>* lhs = traverse(x->lhs);
            Expression<NodeTrait>* rhs = traverse(x->rhs);

            if (lhs->tag == NodeTag::value && rhs->tag == NodeTag::value) {
                Value<NodeTrait>* vlhs = static_cast<Value<NodeTrait>*>(lhs);
                Value<NodeTrait>* vrhs = static_cast<Value<NodeTrait>*>(rhs);;
                return Builder<DummyRoot, Tree>::value(
                    binary_operator(x->name)(vlhs->as<f64>(), vrhs->as<f64>()));
            }

            return Builder<DummyRoot, Tree>::binary_call(x->name, lhs, rhs);
        }

        return_value borrow(Borrow<NodeTrait>* b) {
            return traverse(b->expr);
        }

        Expression<NodeTrait>* value(Value<NodeTrait>* x) {
            return x;
        }

        Expression<NodeTrait>* placeholder(Placeholder<NodeTrait>* x) {
            if (ctx.count(x->name) == 0)
                return x;

            return traverse(ctx.at(x->name));
        }

        return_value arrow(Arrow<NodeTrait>* x) {
            return x;
        }

        return_value type(Type<NodeTrait>* x) {
            return x;
        }

        return_value builtin(Builtin<NodeTrait>* e) {
            return e;
        }

        return_value error(ErrorNode<NodeTrait>* e) {
            return e;
        }

        const Context<NodeTrait>& ctx;
    };
}