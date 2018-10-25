#pragma once

#include "../../Logging/Log.h"

#include "../Builder.h"
#include "../Expression.h"
#include "../Module.h"
#include "../Visitor.h"

#include "Operators.h"

namespace kiwi {
/*/ Functions
// ------------------------------------------------------------------------

// Implementation
// ------------------------------------------------------------------------
class PartialEval : public StaticVisitor<PartialEval, Expression *> {
  public:
    PartialEval(const Context &ctx) : ctx(ctx) {}

    static Expression *run(const Context &ctx, Expression *expr) {
        PartialEval eval(ctx);
        return eval.traverse(expr);
    }

    Expression *functio_decl(FunctionDeclaration *x) { return x->body; }

    Expression *function_call(FunctionCall *x) {
        // we need to build a new context with the arguments

        // lookup the implementation

        // eval body with new context

        // return result
        return x;
    }

    Tuple<i64, String> const &resolve_function_calle(Expression *fun) {
        Expression *callee = traverse(fun);
        if(callee->tag != NodeTag::placeholder) {
            return std::make_tuple(1, "");
        }
        String name = static_cast<Placeholder *>(callee)->name;
        return std::make_tuple(0, name);
    }

    Expression *unary_call(UnaryCall *x) {
        Expression *expr = traverse(x->expr);

        if(expr->tag == NodeTag::value) {
            Value *vexpr = static_cast<Value *>(expr);
            i64 code;
            String name;
            std::tie(code, name) = resolve_function_calle(x->fun);

            if(code > 0) {
                return Builder::error(
                    "function should resolve to an identifier or a function type");
            }
            return Builder::value(unary_operator(name)(vexpr->template as<f64>()));
        }

        return Builder::unary_call(x->fun, expr);
    }

    Expression *binary_call(BinaryCall *x) {
        Expression *lhs = traverse(x->lhs);
        Expression *rhs = traverse(x->rhs);

        u64 code;
        String name;
        std::tie(code, name) = resolve_function_calle(x->fun);

        if(code > 0) {
            return Builder::error("function should resolve to an identifier or a function type");
        }

        if(lhs->tag == NodeTag::value && rhs->tag == NodeTag::value) {
            Value *vlhs = static_cast<Value *>(lhs);
            Value *vrhs = static_cast<Value *>(rhs);

            return Builder::value(
                binary_operator(name)(vlhs->template as<f64>(), vrhs->template as<f64>()));
        }

        return Builder::binary_call(x->fun, lhs, rhs);
    }

    Expression *borrow(Borrow *b) { return traverse(b->expr); }

    Expression *value(Value *x) { return x; }

    Expression *placeholder(Placeholder *x) {
        if(ctx.count(x->name) == 0)
            return x;

        return traverse(ctx.at(x->name));
    }

    Expression *arrow(FunctionType *x) { return x; }

    Expression *type(Type *x) { return x; }

    Expression *builtin(BuiltinType *e) { return e; }

    Expression *error(ErrorNode *e) { return e; }

    const Context &ctx;
};*/
} // namespace kiwi
