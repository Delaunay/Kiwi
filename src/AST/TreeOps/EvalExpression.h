#pragma once

#include "../../Logging/Log.h"
#include "../Expression.h"
#include "../Module.h"
#include "../Root.h"
#include "../Visitor.h"

#include "Operators.h"

namespace kiwi {
/*/ Functions
// ------------------------------------------------------------------------

// Compute the graph (all placeholder must be defined)
double full_eval(const Context &ctx, Expression *expr);

// Implementation
// ------------------------------------------------------------------------
class FullEval : public StaticVisitor<FullEval, double> {
  public:
    FullEval(const Context &ctx) : ctx(ctx) {}

    static double run(const Context &ctx, Expression *expr) {
        FullEval eval(ctx);
        return eval.traverse(expr);
    }

    // nothing to do...
    double function(FunctionDeclaration *) { return 0; }

    double function_call(FunctionCall *x) {
        if(x->fun->tag != NodeTag::placeholder) {
            log_error("Computed Lambda not supported in FullEval Mode");
            return 0;
        }

        Placeholder *pl   = static_cast<Placeholder *>(x->fun);
        std::size_t count = ctx.count(pl->name);

        if(count == 0) {
            log_error("Function does not exist");
            return 0;
        }

        Expression *efun = ctx.at(pl->name);

        if(efun->tag != NodeTag::function_decl) {
            log_error("Calling a non-function");
            return 0;
        }

        FunctionDeclaration *fun = reinterpret_cast<FunctionDeclaration *>(efun);

        if(fun->args_size() != x->args_size()) {
            log_error("argument size mismatch:", fun->args_size(), " ", x->args_size());
            return 0;
        } else {
            // create eval context
            Context fun_ctx;

            for(int i = 0; i < fun->args_size(); ++i) {
                fun_ctx[fun->arg(i)] = x->arg(i);
                log_trace(fun->arg(i), " ", x->arg(i));
            }

            log_trace("Context built ", fun->body);
            return full_eval(fun_ctx, fun->body);
        }
    }

    double binary_call(BinaryCall *x) {
        if(x->fun->tag != NodeTag::placeholder) {
            log_error("Computed Lambda not supported in FullEval Mode");
            return 0;
        }

        Placeholder *pl = static_cast<Placeholder *>(x->fun);

        double a = traverse(x->arg(0));
        double b = traverse(x->arg(1));
        return binary_operator(pl->name)(a, b);
    }

    double unary_call(UnaryCall *x) {
        if(x->fun->tag != NodeTag::placeholder) {
            log_error("Computed Lambda not supported in FullEval Mode");
            return 0;
        }

        Placeholder *pl = static_cast<Placeholder *>(x->fun);

        double a = traverse(x->arg(0));
        return unary_operator(pl->name)(a);
    }

    double borrow(Borrow *b) { return traverse(b->expr); }

    double value(Value *x) { return x->template as<f64>(); }

    double placeholder(Placeholder *x) { return traverse(ctx.at(x->name)); }

    double arrow(FunctionType *) { return 0; }

    double type(Type *) { return 0; }

    double builtin(BuiltinType *) { return 0; }

    double error(ErrorNode *) { return 0; }

    const Context &ctx;
};

template <typename NodeTrait> double full_eval(const Context &ctx, Expression *expr) {
    return FullEval::run(ctx, expr);
}*/
} // namespace kiwi
