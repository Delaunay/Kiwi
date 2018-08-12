#pragma once

#include "../../Logging/Log.h"
#include "../Expression.h"
#include "../Root.h"
#include "../Visitor.h"

#include "../Builder.h"

namespace kiwi {
// Functions
// ------------------------------------------------------------------------

// Copy a tree
template <typename NodeTrait>
Expression<NodeTrait> *copy(Expression<NodeTrait> *expr, bool keep_borrowed = false);

// Implementation
// ------------------------------------------------------------------------
template <typename NodeTrait>
class Copy : public StaticVisitor<Copy<NodeTrait>, NodeTrait, Expression<NodeTrait> *> {
  public:
    typedef typename generic::Root<Expression<NodeTrait>> Root;
    typedef typename generic::DummyRoot<Expression<NodeTrait>> DummyRoot;

    Copy(bool keep_borrowed) : keep_borrowed(keep_borrowed) {}

    static Expression<NodeTrait> *run(Expression<NodeTrait> *expr, bool keep_borrowed = false) {
        if(expr == nullptr)
            return nullptr;

        Copy eval(keep_borrowed);
        return eval.traverse(expr);
    }

    Expression<NodeTrait> *function(Function<NodeTrait> *x) {
        return Builder<NodeTrait>::function(x->name, traverse(x->body));
    }

    Expression<NodeTrait> *function_call(FunctionCall<NodeTrait> *x) {
        std::vector<Expression<NodeTrait> *> new_args;

        std::transform(
            x->args.begin(), x->args.end(),                                  // from -> to
            std::back_inserter(new_args),                                    // insert to
            [this](Expression<NodeTrait> *expr) { return traverse(expr); }); // copy each arguments

        return Builder<NodeTrait>::call(x->name, new_args);
    }

    Expression<NodeTrait> *unary_call(UnaryCall<NodeTrait> *x) {
        return Builder<NodeTrait>::unary_call(x->name, traverse(x->expr));
    }

    Expression<NodeTrait> *binary_call(BinaryCall<NodeTrait> *x) {
        return Builder<NodeTrait>::binary_call(x->name, traverse(x->lhs), traverse(x->rhs));
    }

    Expression<NodeTrait> *value(Value<NodeTrait> *x) {
        return Builder<NodeTrait>::value(x->template as<f64>());
    }

    Expression<NodeTrait> *placeholder(Placeholder<NodeTrait> *x) {
        return Builder<NodeTrait>::placeholder(x->name);
    }

    Expression<NodeTrait> *borrow(Borrow<NodeTrait> *b) {
        if(keep_borrowed)
            return Builder<NodeTrait>::borrow(b->expr);

        return traverse(b->expr);
    }

    // TODO
    Expression<NodeTrait> *arrow(Arrow<NodeTrait> *x) { return x; }

    Expression<NodeTrait> *type(Type<NodeTrait> *x) { return x; }

    Expression<NodeTrait> *builtin(Builtin<NodeTrait> *e) { return e; }

    Expression<NodeTrait> *error(ErrorNode<NodeTrait> *e) { return e; }

    bool keep_borrowed;
};

template <typename NodeTrait>
Expression<NodeTrait> *copy(Expression<NodeTrait> *expr, bool keep_borrowed) {
    return Copy<NodeTrait>::run(expr, keep_borrowed);
}
} // namespace kiwi
