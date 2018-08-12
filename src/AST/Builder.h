#pragma once

#include <cmath>

#include "LightAST.h"
#include "Root.h"
#include "TreeOps.h"

namespace kiwi {

template <typename NodeTrait> class Builder {
  public:
    NODE_TYPES;

    using ExpressionPtr = generic::Root<Expression<NodeTrait>>;
    using TypePtr       = generic::Root<Type<NodeTrait>>;

    static ExpressionPtr function(String const &op, const ExpressionPtr &body) {
        Function<NodeTrait> *root = new Function<NodeTrait>(op, body.take_ownership());

        PARENT(body->parent = root);
        return ExpressionPtr(root);
    }

    static ExpressionPtr function(const String op, const ExpressionPtr &body, const TypePtr &type) {
        if(type) {
            assert(type->tag == NodeTypeTag::arrow && "Function Type is represented by an arrow");
        }

        Arrow<NodeTrait> *arrow   = static_cast<Arrow<NodeTrait> *>(type.take_ownership());
        Function<NodeTrait> *root = new Function<NodeTrait>(op, body.take_ownership(), arrow);

        PARENT(body->parent = root);
        PARENT(type->parent = root);
        return ExpressionPtr(root);
    }

    static ExpressionPtr call(ExpressionPtr const op, Array<Expression<NodeTrait> *> args) {
        return ExpressionPtr(new FunctionCall<NodeTrait>(op, args));
    }

    static ExpressionPtr unary_call(ExpressionPtr const op, const ExpressionPtr &expr) {
        return new UnaryCall<NodeTrait>(op, expr.take_ownership());
    }

    static ExpressionPtr binary_call(ExpressionPtr const op, const ExpressionPtr &lhs,
                                     const ExpressionPtr &rhs) {
        return new BinaryCall<NodeTrait>(op, lhs.take_ownership(), rhs.take_ownership());
    }

    static ExpressionPtr error(String const &message) { return new ErrorNode<NodeTrait>(message); }

    static ExpressionPtr value(double value) { return new Value<NodeTrait>(value); }

    static ExpressionPtr placeholder(String const &name) {
        return new Placeholder<NodeTrait>(name);
    }

    static ExpressionPtr borrow(const ExpressionPtr &expr) {
        return new Borrow<NodeTrait>(expr.get());
    }

    static TypePtr arrow(Array<Type<NodeTrait> *> args, const TypePtr &return_type) {
        return new Arrow<NodeTrait>(args, return_type);
    }

    static TypePtr builtin(String const &name) { return new Builtin<NodeTrait>(name); }

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

        ExpressionPtr root = new BinaryCall<NodeTrait>(new Placeholder<NodeTrait>("+"),
                                                       lhs.take_ownership(), rhs.take_ownership());
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

        ExpressionPtr root = new BinaryCall<NodeTrait>(new Placeholder<NodeTrait>("*"),
                                                       lhs.take_ownership(), rhs.take_ownership());
        PARENT(lhs->parent = root);
        PARENT(rhs->parent = root);
        return root;
    }

    static ExpressionPtr div(const ExpressionPtr &lhs, const ExpressionPtr &rhs) {
        ExpressionPtr root = new BinaryCall<NodeTrait>(new Placeholder<NodeTrait>("/"),
                                                       lhs.take_ownership(), rhs.take_ownership());
        PARENT(lhs->parent = root);
        PARENT(rhs->parent = root);
        return root;
    }

    static ExpressionPtr sub(const ExpressionPtr &lhs, const ExpressionPtr &rhs) {
        ExpressionPtr root = new BinaryCall<NodeTrait>(new Placeholder<NodeTrait>("-"),
                                                       lhs.take_ownership(), rhs.take_ownership());
        PARENT(lhs->parent = root);
        PARENT(rhs->parent = root);
        return root;
    }
};
} // namespace kiwi
