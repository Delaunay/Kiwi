#ifndef KIWI_AST_COPY_HEADER
#define KIWI_AST_COPY_HEADER

#include "../../Logging/Log.h"
#include "../Builder.h"
#include "../Visitor.h"

namespace kiwi {
/* / Functions
// ------------------------------------------------------------------------

// Copy a tree

Expression *copy(Expression *expr, bool keep_borrowed = false);

// Implementation
// ------------------------------------------------------------------------
class Copy : public StaticVisitor<Copy, Expression *> {
  public:
    Copy(bool keep_borrowed) : keep_borrowed(keep_borrowed) {}

    static Expression *run(Expression *expr, bool keep_borrowed = false) {
        if(expr == nullptr)
            return nullptr;

        Copy eval(keep_borrowed);
        return eval.traverse(expr);
    }

    Expression *function(FunctionDeclaration *x) {
        return Builder::function(x->name, traverse(x->body));
    }

    Expression *function_call(FunctionCall *x) {
        std::vector<Expression *> new_args;

        std::transform(x->args.begin(), x->args.end(),                       // from -> to
                       std::back_inserter(new_args),                         // insert to
                       [this](Expression *expr) { return traverse(expr); }); // copy each arguments

        return Builder::call(x->fun, new_args);
    }

    Expression *unary_call(UnaryCall *x) { return Builder::unary_call(x->fun, traverse(x->expr)); }

    Expression *binary_call(BinaryCall *x) {
        return Builder::binary_call(x->fun, traverse(x->lhs), traverse(x->rhs));
    }

    Expression *value(Value *x) { return Builder::value(x->template as<f64>()); }

    Expression *placeholder(Placeholder *x) { return Builder::placeholder(x->name); }

    Expression *borrow(Borrow *b) {
        if(keep_borrowed)
            return nullptr; // Builder::borrow(b->expr);

        return traverse(b->expr);
    }

    // TODO
    Node *function_type(FunctionType *x) { return x; }

    Node *type(Type *x) { return x; }

    Node *builtin_type(BuiltinType *e) { return e; }

    Node *error(ErrorNode *e) { return e; }

    bool keep_borrowed;
};

Expression *copy(Expression *expr, bool keep_borrowed) { return Copy::run(expr, keep_borrowed); } */
} // namespace kiwi
#endif
