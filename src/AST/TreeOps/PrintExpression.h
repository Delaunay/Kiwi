#pragma once

#include <ostream>

#include "../../Logging/Log.h"
#include "../Expression.h"
#include "../Root.h"
#include "../Visitor.h"

namespace kiwi {
/*/ Functions
// ------------------------------------------------------------------------

std::ostream &print_expr(std::ostream &out, Expression *expr);

std::ostream &operator<<(std::ostream &out, Expression *expr);

// Implementation
// ------------------------------------------------------------------------
class Printing : public StaticVisitor<Printing, void, int> {
  public:
    Printing(std::ostream &out) : out(out) {}

    static std::ostream &run(std::ostream &out, Expression *expr) {
        Printing eval(out);
        eval.traverse(expr, 1);
        return out;
    }

    void function_declaration(FunctionDeclaration *x, int indentation) {
        std::size_t n = x->args_size();
        out << "def " << x->name << "(";

        // try to resolve the function type to its Natural -> type
        Expression *etype   = x->type;
        FunctionType *ftype = nullptr;
        if(etype->tag != NodeTag::function_type) {
            ftype = static_cast<FunctionType *>(etype);
        }

        if(n > 0) {
            for(std::size_t i = 0; i < n - 1; ++i) {

                out << x->arg(i);
                if(ftype) {
                    out << ": ";
                    traverse(ftype->arg(i), indentation);
                }

                out << ", ";
            }
            out << x->arg(n - 1);
            if(ftype) {
                out << ": ";
                traverse(ftype->arg(n - 1), indentation);
            }
        }

        out << ") -> ";

        if(ftype) {
            traverse(ftype->return_type, indentation);
        } else if(x->type) {
            traverse(x->type, indentation);
        }

        out << ":\n" << std::string(indentation * 4, ' ');

        traverse(x->body, indentation + 1);
    }

    void function_call(FunctionCall *x, int indentation) {
        size_t n = x->args_size() - 1;

        traverse(x->fun, indentation);
        out << "(";
        for(std::size_t i = 0; i < n; ++i) {
            traverse(x->arg(i), indentation);
            out << ", ";
        }

        traverse(x->arg(n), indentation);
        out << ")";
    }

    void unary_call(UnaryCall *x, int indentation) {
        if(!x->right)
            traverse(x->fun, indentation);

        traverse(x->arg(0), indentation);

        if(x->right)
            traverse(x->fun, indentation);
    }

    void binary_call(BinaryCall *x, int indentation) {
        out << "(";
        traverse(x->arg(0), indentation);
        out << " ";
        traverse(x->fun, indentation);
        out << " ";
        traverse(x->arg(1), indentation);
        out << ")";
    }

    void borrow(Borrow *b, int indentation) { return traverse(b->expr, indentation); }

    void error(ErrorNode *e, int) { out << "(error, m = " << e->message << ")"; }

    void value(Value *x, int) {
        x->print(out);
        // out << x->as<f64>();
    }

    void placeholder(Placeholder *x, int) { out << x->name; }

    std::ostream &out;
};

std::ostream &print_expr(std::ostream &out, Expression *expr) { return Printing::run(out, expr); }

std::ostream &operator<<(std::ostream &out, Expression *expr) { return print_expr(out, expr); }*/
} // namespace kiwi
