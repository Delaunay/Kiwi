#pragma once

#include <ostream>

#include "../../Logging/Log.h"
#include "../Expression.h"
#include "../Root.h"
#include "../Visitor.h"

namespace kiwi {
// Functions
// ------------------------------------------------------------------------

template <typename NodeTrait>
std::ostream &print_expr(std::ostream &out, Expression<NodeTrait> *expr);

template <typename NodeTrait>
std::ostream &operator<<(std::ostream &out, Expression<NodeTrait> *expr);

// Implementation
// ------------------------------------------------------------------------
template <typename NodeTrait>
class TypePrinting : public StaticTypeVisitor<TypePrinting<NodeTrait>, NodeTrait, void, int> {
  public:
    TypePrinting(std::ostream &out) : out(out) {}

    static std::ostream &run(std::ostream &out, Type<NodeTrait> *expr, int idt = 1) {
        TypePrinting eval(out);

        if(!expr) {
            log_error("No Type to print");
            return out;
        }

        eval.traverse(expr, idt);
        return out;
    }

    void arrow(Arrow<NodeTrait> *x, int idt) {
        out << '(';

        for(std::size_t i = 0; i < x->args.size() - 1; ++i) {
            traverse(x->args[i], idt);
            out << ", ";
        }

        traverse(x->args.back(), idt);

        out << ") -> ";

        traverse(x->return_type, idt);
    }

    void builtin(Builtin<NodeTrait> *e, int) { out << NodeTrait::to_string(e->name); }

    void error(ErrorType<NodeTrait> *e, int) {}

    void unkown(Unknown<NodeTrait> *e, int) {}

    void record(Record<NodeTrait> *e, int) {}

    void error(ErrorNode<NodeTrait> *e, int) {}

  private:
    std::ostream &out;
};

template <typename NodeTrait>
class Printing : public StaticVisitor<Printing<NodeTrait>, NodeTrait, void, int> {
  public:
    Printing(std::ostream &out) : out(out) {}

    static std::ostream &run(std::ostream &out, Expression<NodeTrait> *expr) {
        Printing eval(out);
        eval.traverse(expr, 1);
        return out;
    }

    void function(Function<NodeTrait> *x, int indentation) {
        int n = int(x->args_size()) - 1;
        out << "def " << x->name << "(";

        if(n >= 0) {
            for(int i = 0; i < n; ++i) {

                out << x->arg(i);
                if(x->type) {
                    out << ": ";
                    TypePrinting<NodeTrait>::run(out, x->type->arg(i), indentation);
                }

                out << ", ";
            }
            out << x->arg(n);
            if(x->type) {
                out << ": ";
                TypePrinting<NodeTrait>::run(out, x->type->arg(n), indentation);
            }
        }

        out << ")";

        if(x->type) {
            out << " -> ";
            TypePrinting<NodeTrait>::run(out, x->type->return_type, indentation);
        }

        out << ":\n" << std::string(indentation * 4, ' ');

        traverse(x->body, indentation + 1);
    }

    void function_call(FunctionCall<NodeTrait> *x, int indentation) {
        size_t n = x->args_size() - 1;

        traverse(x->fun, indentation);
        out << "(";
        for(int i = 0; i < n; ++i) {
            traverse(x->arg(i), indentation);
            out << ", ";
        }

        traverse(x->arg(n), indentation);
        out << ")";
    }

    void unary_call(UnaryCall<NodeTrait> *x, int indentation) {
        if(!x->right)
            traverse(x->fun, indentation);

        traverse(x->arg(0), indentation);

        if(x->right)
            traverse(x->fun, indentation);
    }

    void binary_call(BinaryCall<NodeTrait> *x, int indentation) {
        out << "(";
        traverse(x->arg(0), indentation);
        out << " ";
        traverse(x->fun, indentation);
        out << " ";
        traverse(x->arg(1), indentation);
        out << ")";
    }

    void borrow(Borrow<NodeTrait> *b, int indentation) { return traverse(b->expr, indentation); }

    void error(ErrorNode<NodeTrait> *e, int indentation) {
        out << "(error, m = " << e->message << ")";
    }

    void value(Value<NodeTrait> *x, int) {
        x->print(out);
        // out << x->as<f64>();
    }

    void placeholder(Placeholder<NodeTrait> *x, int) { out << x->name; }

    std::ostream &out;
};

template <typename NodeTrait>
std::ostream &print_expr(std::ostream &out, Expression<NodeTrait> *expr) {
    return Printing<NodeTrait>::run(out, expr);
}

template <typename NodeTrait> std::ostream &print_type(std::ostream &out, Type<NodeTrait> *expr) {
    return TypePrinting<NodeTrait>::run(out, expr);
}

template <typename NodeTrait>
std::ostream &operator<<(std::ostream &out, Expression<NodeTrait> *expr) {
    return print_expr(out, expr);
}
} // namespace kiwi
