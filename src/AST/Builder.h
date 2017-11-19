#pragma once

#include <cmath>

#include "LightAST.h"
#include "TreeOperations.h"
#include "Root.h"

namespace kiwi {

template<typename T, typename Tree>
class Builder{
public:
    typedef T Parent;
    typedef typename T::ptr_type Expr;

    typedef typename Tree::Call Call;
    typedef typename Tree::Node Node;
    #define X(name, object) typedef typename Tree::object object;
        KIWI_AST_NODES
    #undef X

    static Parent function(const std::string op, const Parent& body){
        Function* root = new Function(op, body.take_ownership());
        // append_args(root, body.get());
        PARENT(body->parent = root);
        return Parent(root);
    }

    static Parent function(const std::string op, const Parent& body, const Parent& type){
        Function* root = new Function(op, body.take_ownership(), type.take_ownership());
        PARENT(body->parent = root);
        return Parent(root);
    }

    static Parent call(const std::string& op, std::vector<Expr> args){
        return new FunctionCall(op, args);
    }
    static Parent unary_call(const std::string& op, const Parent& expr){
        return new UnaryCall(op, expr.take_ownership());
    }
    static Parent binary_call(const std::string& op,
                              const Parent& lhs, const Parent& rhs){
        return new BinaryCall(op, lhs.take_ownership(), rhs.take_ownership());
    }

    static Parent error(const std::string& message){
        return new ErrorNode(message);
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

    static Parent arrow(std::vector<Node*> args, const Parent& return_type){
        return new Arrow(args, return_type);
    }

    static Parent builtin(const std::string& name){
        return new Builtin(name);
    }


	bool is_zero(f64 value) {
		return std::abs(as<f64>(value)) <= 1e-12;
	}

	bool is_one(f64 value) {
		return is_zero(value - 1);
	}

    static Parent add(const Parent& lhs, const Parent& rhs){
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

        Parent root = new BinaryCall("+", lhs.take_ownership(), rhs.take_ownership());
        PARENT(lhs->parent = root);
        PARENT(rhs->parent = root);
        return root;
    }

    static Parent mult(const Parent& lhs, const Parent& rhs){
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

        Parent root = new BinaryCall("*", lhs.take_ownership(), rhs.take_ownership());
        PARENT(lhs->parent = root);
        PARENT(rhs->parent = root);
        return root;
    }

    static Parent div(const Parent& lhs, const Parent& rhs){
        Parent root = new BinaryCall("/", lhs.take_ownership(), rhs.take_ownership());
        PARENT(lhs->parent = root);
        PARENT(rhs->parent = root);
        return root;
    }

    static Parent sub(const Parent& lhs, const Parent& rhs){
        Parent root = new BinaryCall("-", lhs.take_ownership(), rhs.take_ownership());
        PARENT(lhs->parent = root);
        PARENT(rhs->parent = root);
        return root;
    }
};


}
