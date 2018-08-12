#pragma once

#include "../../Logging/Log.h"
#include "../Expression.h"
#include "../Root.h"
#include "../Visitor.h"

#include "../Builder.h"

namespace kiwi {
// Functions
// ------------------------------------------------------------------------

// Implementation
// ------------------------------------------------------------------------
template <typename NodeTrait>
class PartialEval : public StaticVisitor<PartialEval<NodeTrait>, NodeTrait,
                                         generic::Root<Expression<NodeTrait>>> {
  public:
    typedef typename generic::Root<Expression<NodeTrait>> Root;
    typedef typename generic::DummyRoot<Expression<NodeTrait>> DummyRoot;

    PartialEval(const Context<NodeTrait> &ctx) : ctx(ctx) {}

    static Root run(const Context<NodeTrait> &ctx, Expression<NodeTrait> *expr) {
        PartialEval eval(ctx);
        return eval.traverse(expr);
    }

    Expression<NodeTrait> *function(Function<NodeTrait> *x) { return x; }

    Expression<NodeTrait> *function_call(FunctionCall<NodeTrait> *x) {
        // we need to build a new context with the arguments

        // lookup the implementation

        // eval body with new context

        // return result
        return x;
    }

    Expression<NodeTrait> *unary_call(UnaryCall<NodeTrait> *x) {
        Expression<NodeTrait> *expr = traverse(x->expr);

        if(expr->tag == NodeTag::value) {
            Value<NodeTrait> *vexpr = static_cast<Value<NodeTrait> *>(expr);
            return Builder<NodeTrait>::value(unary_operator(x->name)(vexpr->template as<f64>()));
        }

        return Builder<NodeTrait>::unary_call(x->name, expr);
    }

    Expression<NodeTrait> *binary_call(BinaryCall<NodeTrait> *x) {
        Expression<NodeTrait> *lhs = traverse(x->lhs);
        Expression<NodeTrait> *rhs = traverse(x->rhs);

        if(lhs->tag == NodeTag::value && rhs->tag == NodeTag::value) {
            Value<NodeTrait> *vlhs = static_cast<Value<NodeTrait> *>(lhs);
            Value<NodeTrait> *vrhs = static_cast<Value<NodeTrait> *>(rhs);
            ;
            return Builder<DummyRoot>::value(
                binary_operator(x->name)(vlhs->template as<f64>(), vrhs->template as<f64>()));
        }

        return Builder<NodeTrait>::binary_call(x->name, lhs, rhs);
    }

    Expression<NodeTrait> *borrow(Borrow<NodeTrait> *b) { return traverse(b->expr); }

    Expression<NodeTrait> *value(Value<NodeTrait> *x) { return x; }

    Expression<NodeTrait> *placeholder(Placeholder<NodeTrait> *x) {
        if(ctx.count(x->name) == 0)
            return x;

        return traverse(ctx.at(x->name));
    }

    Expression<NodeTrait> *arrow(Arrow<NodeTrait> *x) { return x; }

    Expression<NodeTrait> *type(Type<NodeTrait> *x) { return x; }

    Expression<NodeTrait> *builtin(Builtin<NodeTrait> *e) { return e; }

    Expression<NodeTrait> *error(ErrorNode<NodeTrait> *e) { return e; }

    const Context<NodeTrait> &ctx;
};
} // namespace kiwi
