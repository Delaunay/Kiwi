#pragma once

#include "../../Logging/Log.h"
#include "../Expression.h"
#include "../Root.h"
#include "../Visitor.h"

#include "Operators.h"

namespace kiwi {
template <typename NodeTrait> using Context = Dict<String, Expression<NodeTrait> *>;

// Functions
// ------------------------------------------------------------------------

// Compute the graph (all placeholder must be defined)
template <typename NodeTrait>
double full_eval(const Context<NodeTrait> &ctx, Expression<NodeTrait> *expr);

// Implementation
// ------------------------------------------------------------------------
template <typename NodeTrait>
class FullEval : public StaticVisitor<FullEval<NodeTrait>, NodeTrait, double> {
  public:
    FullEval(const Context<NodeTrait> &ctx) : ctx(ctx) {}

    static double run(const Context<NodeTrait> &ctx, Expression<NodeTrait> *expr) {
        FullEval eval(ctx);
        return eval.traverse(expr);
    }

    // nothing to do...
    double function(Function<NodeTrait> *) { return 0; }

    double function_call(FunctionCall<NodeTrait> *x) {
        if(x->fun->tag != NodeTag::placeholder) {
            log_error("Computed Lambda not supported in FullEval Mode");
            return 0;
        }

        Placeholder<NodeTrait> *pl = static_cast<Placeholder<NodeTrait> *>(x->fun);
        std::size_t count          = ctx.count(pl->name);

        if(count == 0) {
            log_error("Function does not exist");
            return 0;
        }

        Expression<NodeTrait> *efun = ctx.at(pl->name);

        if(efun->tag != NodeTag::function) {
            log_error("Calling a non-function");
            return 0;
        }

        Function<NodeTrait> *fun = static_cast<Function<NodeTrait> *>(efun);

        if(fun->args_size() != x->args_size()) {
            log_error("argument size mismatch:", fun->args_size(), " ", x->args_size());
            return 0;
        } else {
            // create eval context
            Context<NodeTrait> fun_ctx;

            for(int i = 0; i < fun->args_size(); ++i) {
                fun_ctx[fun->arg(i)] = x->arg(i);
                log_trace(fun->arg(i), " ", x->arg(i));
            }

            log_trace("Context built ", fun->body);
            return full_eval(fun_ctx, fun->body);
        }
    }

    double binary_call(BinaryCall<NodeTrait> *x) {
        if(x->fun->tag != NodeTag::placeholder) {
            log_error("Computed Lambda not supported in FullEval Mode");
            return 0;
        }

        Placeholder<NodeTrait> *pl = static_cast<Placeholder<NodeTrait> *>(x->fun);

        double a = traverse(x->arg(0));
        double b = traverse(x->arg(1));
        return binary_operator(pl->name)(a, b);
    }

    double unary_call(UnaryCall<NodeTrait> *x) {
        if(x->fun->tag != NodeTag::placeholder) {
            log_error("Computed Lambda not supported in FullEval Mode");
            return 0;
        }

        Placeholder<NodeTrait> *pl = static_cast<Placeholder<NodeTrait> *>(x->fun);

        double a = traverse(x->arg(0));
        return unary_operator(pl->name)(a);
    }

    double borrow(Borrow<NodeTrait> *b) { return traverse(b->expr); }

    double value(Value<NodeTrait> *x) { return x->template as<f64>(); }

    double placeholder(Placeholder<NodeTrait> *x) { return traverse(ctx.at(x->name)); }

    double arrow(Arrow<NodeTrait> *) { return 0; }

    double type(Type<NodeTrait> *) { return 0; }

    double builtin(Builtin<NodeTrait> *) { return 0; }

    double error(ErrorNode<NodeTrait> *) { return 0; }

    const Context<NodeTrait> &ctx;
};

template <typename NodeTrait>
double full_eval(const Context<NodeTrait> &ctx, Expression<NodeTrait> *expr) {
    return FullEval<NodeTrait>::run(ctx, expr);
}
} // namespace kiwi
