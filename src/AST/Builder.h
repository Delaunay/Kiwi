#pragma once

#include <cmath>
#include "Expression.h"

namespace kiwi {

class Builder{
public:
    static Expr add(Expr lhs, Expr rhs){
        // Constant folding
        if (lhs->tag == NodeTag::value){
            Value* vlhs = static_cast<Value*>(lhs);
            if (std::abs(vlhs->value) <= 1e-12){
                return rhs;
            }

            if (rhs->tag == NodeTag::value){
                Value* vrhs = static_cast<Value*>(rhs);
                return new Value(vrhs->value + vlhs->value);
            }
        }

        if (rhs->tag == NodeTag::value){
            Value* vrhs = static_cast<Value*>(rhs);
            if (std::abs(vrhs->value) <= 1e-12){
                return lhs;
            }
        }


        Expr root = new Add(lhs, rhs);
        PARENT(lhs->parent = root);
        PARENT(rhs->parent = root);
        return root;
    }

    static Expr value(double value){
        return new Value(value);
    }

    static Expr placeholder(const std::string& name){
        return new Placeholder(name);
    }
};


}
