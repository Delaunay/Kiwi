#pragma once

#include <ostream>

#include "../../Logging/Log.h"
#include "../Expression.h"
#include "../Root.h"
#include "../Visitor.h"

namespace kiwi {

class PrintType : public TypeVisitor<std::ostream &, std::ostream &, u64> {
  public:
    std::ostream &builtin_type(BuiltinType *, std::ostream &out, u64 depth) override;
    std::ostream &function_type(FunctionType *, std::ostream &out, u64 depth) override;
    std::ostream &union_type(UnionType *, std::ostream &out, u64 depth) override;
    std::ostream &struct_type(StructType *, std::ostream &out, u64 depth) override;

    std::ostream &unhandled_type(Type *, std::ostream &out, u64 depth) override;
    std::ostream &nullptr_type(std::ostream &out, u64 depth) override;
};

class PrintExpression : public ExpressionVisitor<std::ostream &, std::ostream &, u64> {
  public:
    std::ostream &unary_call(UnaryCall *, std::ostream &out, u64 depth) override;
    std::ostream &binary_call(BinaryCall *, std::ostream &out, u64 depth) override;
    std::ostream &function_call(FunctionCall *, std::ostream &out, u64 depth) override;
    std::ostream &match(Match *, std::ostream &out, u64 depth) override;
    std::ostream &block(Block *, std::ostream &out, u64 depth) override;
    std::ostream &placeholder(Placeholder *, std::ostream &out, u64 depth) override;
    std::ostream &value(Value *, std::ostream &out, u64 depth) override;

    std::ostream &unhandled_expression(Expression *, std::ostream &out, u64 depth) override;
    std::ostream &nullptr_expression(std::ostream &out, u64 depth) override;
};

class PrintDefinition : public DefinitionVisitor<std::ostream &, std::ostream &, u64> {
  public:
    std::ostream &function_def(FunctionDefinition *, std::ostream &out, u64 depth) override;
    std::ostream &macro_def(MacroDefinition *, std::ostream &out, u64 depth) override;
    std::ostream &struct_def(StructDefinition *, std::ostream &out, u64 depth) override;
    std::ostream &union_def(UnionDefinition *, std::ostream &out, u64 depth) override;

    std::ostream &unhandled_definition(Definition *, std::ostream &out, u64 depth) override;
    std::ostream &nullptr_definition(std::ostream &out, u64 depth) override;
};

class PrintStatement : public StatementVisitor<std::ostream &, std::ostream &, u64> {
  public:
    std::ostream &visit_type(Type *type, std::ostream &out, u64 depth) override {
        return PrintType().visit_type(type, out, depth);
    }
    std::ostream &visit_definition(Definition *def, std::ostream &out, u64 depth) override {
        return PrintDefinition().visit_definition(def, out, depth);
    }
    std::ostream &visit_expression(Expression *expr, std::ostream &out, u64 depth) override {
        return PrintExpression().visit_expression(expr, out, depth);
    }

    std::ostream &unhandled_statement(Statement *stmt, std::ostream &out, u64) override {
        log_info(to_string(stmt->tag));
        return out;
    }

    std::ostream &nullptr_statement(std::ostream &out, u64) override {
        return out << "NoneStatement(nullptr)";
    }
};

/*/ Functions
// ------------------------------------------------------------------------

std::ostream &Print_expr(std::ostream &out, Expression *expr);

std::ostream &operator<<(std::ostream &out, Expression *expr);

// Implementation
// ------------------------------------------------------------------------
class Printing : public StaticVisitor<Printing, void, u64> {
  public:
    Printing(std::ostream &out) : out(out) {}

    static std::ostream &run(std::ostream &out, Expression *expr) {
        Printing eval(out);
        eval.traverse(expr, 1);
        return out;
    }

    void function_declaration(FunctionDeclaration *x, u64 indentation) {
        std::size_t n = x->args_size();
        out << "def " << x->name << "(";

        // try to resolve the function type to its Natural -> type
        Expression *etype   = x->type;
        FunctionType *ftype = nullptr;
        if(etype->tag != NodeTag::function_type) {
            ftype = static_cast<FunctionType *>(etype);
        }

        if(n > 0) {
            for(std::size_t i; i < n - 1; ++i) {

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

    void function_call(FunctionCall *x, u64 indentation) {
        size_t n = x->args_size() - 1;

        traverse(x->fun, indentation);
        out << "(";
        for(std::size_t i; i < n; ++i) {
            traverse(x->arg(i), indentation);
            out << ", ";
        }

        traverse(x->arg(n), indentation);
        out << ")";
    }

    void unary_call(UnaryCall *x, u64 indentation) {
        if(!x->right)
            traverse(x->fun, indentation);

        traverse(x->arg(0), indentation);

        if(x->right)
            traverse(x->fun, indentation);
    }

    void binary_call(BinaryCall *x, u64 indentation) {
        out << "(";
        traverse(x->arg(0), indentation);
        out << " ";
        traverse(x->fun, indentation);
        out << " ";
        traverse(x->arg(1), indentation);
        out << ")";
    }

    void borrow(Borrow *b, u64 indentation) { return traverse(b->expr, indentation); }

    void error(ErrorNode *e, u64) { out << "(error, m = " << e->message << ")"; }

    void value(Value *x, u64) {
        x->Print(out);
        // out << x->as<f64>();
    }

    void placeholder(Placeholder *x, u64) { out << x->name; }

    std::ostream &out;
};

std::ostream &Print_expr(std::ostream &out, Expression *expr) { return Printing::run(out, expr); }

std::ostream &operator<<(std::ostream &out, Expression *expr) { return Print_expr(out, expr); }*/
} // namespace kiwi
