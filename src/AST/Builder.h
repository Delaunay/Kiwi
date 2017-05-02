#pragma once

#include <cmath>

#include "Expression.h"
#include "TreeOperations.h"
#include "Root.h"

namespace kiwi {

template<typename T = Root>
class Builder{
public:
    typedef T Parent;
    /*
    static Parent add(const Parent& lhs, const Parent& rhs){

        // Constant folding
        if (lhs->tag == NodeTag::value){
            Value* vlhs = static_cast<Value*>(lhs.get());
            if (std::abs(vlhs->value) <= 1e-12){
                return rhs;
            }

            if (rhs->tag == NodeTag::value){
                Value* vrhs = static_cast<Value*>(rhs.get());
                return new Value(vrhs->value + vlhs->value);
            }
        }

        if (rhs->tag == NodeTag::value){
            Value* vrhs = static_cast<Value*>(rhs.get());
            if (std::abs(vrhs->value) <= 1e-12){
                return lhs;
            }
        }

        Parent root = new Add(lhs.take_ownership(),
                              rhs.take_ownership());

        PARENT(lhs->parent = root);
        PARENT(rhs->parent = root);
        return root;
    }*/

    static Parent unary(const std::string op, const Parent& expr){
        Parent root = new UnaryFunction(op, expr.take_ownership());
        PARENT(expr->parent = root);
        return root;
    }
    static Parent binary(const std::string op, const Parent& lhs, const Parent& rhs){
        Parent root = new BinaryFunction(op,
                                         lhs.take_ownership(),
                                         rhs.take_ownership());
        PARENT(lhs->parent = root);
        PARENT(rhs->parent = root);
        return root;
    }
    static Parent nnary(const std::string name){
        return new NnaryFunction(name);
    }

    static Parent value(double value){
        return new Value(value);
    }

    static Parent placeholder(const std::string& name){
        return new Placeholder(name);
    }

    static Parent borrow(const Parent& expr){
        return new Borrow(expr.get());
    }
};


}
