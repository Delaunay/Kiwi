#ifndef KIWI_AST_BUILDER_HEADER
#define KIWI_AST_BUILDER_HEADER

#define PARENT(x)

#include "Definition.h"
#include "Expression.h"
#include "Type.h"
#include "Value.h"
#include <cmath>

namespace kiwi {

class Builder {
  public:
    using ExpressionPtr = Expression *; // UniquePtr<Expression>;
    using TypePtr       = Type *;       // UniquePtr<Type>;

    static Definition *function(String const &op, const ExpressionPtr &body) {
        FunctionDefinition *root = new FunctionDefinition(op, body);

        PARENT(body->parent = root);
        return root;
    }

    /*
    static ExpressionPtr function(String const &op, const ExpressionPtr &body,
                                  const TypePtr &type) {
        FunctionType *arrow      = static_cast<FunctionType *>(type);
        FunctionDefinition *root = new FunctionDefinition(op, body, arrow);

        PARENT(body->parent = root);
        PARENT(type->parent = root);
        return ExpressionPtr(root);
    }*/

    static ExpressionPtr call(ExpressionPtr const op, Array<Expression *> args) {
        return ExpressionPtr(new FunctionCall(op, args));
    }

    static ExpressionPtr unary_call(ExpressionPtr const op, const ExpressionPtr &expr) {
        return new UnaryCall(op, expr);
    }

    static ExpressionPtr binary_call(ExpressionPtr const op, const ExpressionPtr &lhs,
                                     const ExpressionPtr &rhs) {
        return new BinaryCall(op, lhs, rhs);
    }

    static ExpressionPtr error(String const &message) { return new ErrorNode(message); }

    static ExpressionPtr value(double value) { return new Value(value); }

    static ExpressionPtr placeholder(String const &name) { return new Placeholder(name); }

    // static ExpressionPtr borrow(Node *expr) { return new Borrow(expr); }

    static TypePtr function_type(Array<Type *> args, const TypePtr &return_type) {
        return new FunctionType(args, return_type);
    }

    static TypePtr arrow(Array<Type *> args, const TypePtr &return_type) {
        return function_type(args, return_type);
    }

    static ExpressionPtr block() { return new Block(); }

    static TypePtr builtin(String const &name) { return new BuiltinType(name); }

    bool is_zero(f64 value) { return std::abs(value) <= 1e-12; }

    bool is_one(f64 value) { return is_zero(value - 1); }

    static ExpressionPtr add(const ExpressionPtr &lhs, const ExpressionPtr &rhs) {
        /*/ Constant folding
        if (lhs->tag == NodeTag::value){
            Value* vlhs = static_cast<Value*>(lhs.get());
            if (is_zero(as<f64>(vlhs))){
                return rhs;
            }

            if (rhs->tag == NodeTag::value){
                Value* vrhs = static_cast<Value*>(rhs.get());
                return value(as<f64>(vrhs) + as<f64>(vlhs));
            }
        }

        if (rhs->tag == NodeTag::value){
            Value* vrhs = static_cast<Value*>(rhs.get());
            if (is_zero(as<f64>(vrhs))){
                return lhs;
            }
        }*/

        ExpressionPtr root = binary_call(placeholder("+"), lhs, rhs);
        PARENT(lhs->parent = root);
        PARENT(rhs->parent = root);
        return root;
    }

    static ExpressionPtr mult(const ExpressionPtr &lhs, const ExpressionPtr &rhs) {
        /*/ Constant folding
        if (lhs->tag == NodeTag::value) {
                Value* vlhs = static_cast<Value*>(lhs.get());
                f64 flhs = as<f64>(vlhs);

                if (is_one(flhs)) {
                        return rhs;
                }

                if (is_zero(flhs)) {
                        return value(0);
                }

                if (rhs->tag == NodeTag::value) {
                        Value* vrhs = static_cast<Value*>(rhs.get());
                        return value(as<f64>(vrhs) * flhs);
                }
        }

        if (rhs->tag == NodeTag::value) {
                Value* vrhs = static_cast<Value*>(rhs.get());
                f64 frhs = as<f64>(vrhs);

                if (is_one(frhs)) {
                        return lhs;
                }

                if (is_zero(frhs)) {
                        return value(0);
                }
        }*/

        ExpressionPtr root = binary_call(placeholder("*"), lhs, rhs);
        PARENT(lhs->parent = root);
        PARENT(rhs->parent = root);
        return root;
    }

    static ExpressionPtr div(const ExpressionPtr &lhs, const ExpressionPtr &rhs) {
        ExpressionPtr root = binary_call(placeholder("/"), lhs, rhs);
        PARENT(lhs->parent = root);
        PARENT(rhs->parent = root);
        return root;
    }

    static ExpressionPtr sub(const ExpressionPtr &lhs, const ExpressionPtr &rhs) {
        ExpressionPtr root = binary_call(placeholder("-"), lhs, rhs);
        PARENT(lhs->parent = root);
        PARENT(rhs->parent = root);
        return root;
    }

    static Definition *struct_def(String const &name, Array<Tuple<String, Statement *>> const &meta,
                                  Array<Tuple<String, Statement *>> const &attr) {
        return new StructDefinition(name, meta, attr);
    }

    static Definition *union_def(String const &name, Array<Tuple<String, Statement *>> const &meta,
                                 Array<Tuple<String, Statement *>> const &attr) {
        return new UnionDefinition(name, meta, attr);
    }
};
} // namespace kiwi

#endif
