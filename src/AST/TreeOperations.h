#pragma once

#include <ostream>
#include <unordered_map>

#include <iterator>
#include <algorithm>
#include <functional>

#include "Root.h"
#include "Expression.h"
#include "Visitor.h"
//#include "TreeOperations.h"
//#include "Builder.h"

#include "../Logging/Log.h"

namespace kiwi{

template<typename NodeTrait>
using Context = std::unordered_map<std::string, Expression<NodeTrait>*>;

template<typename NodeTrait>
std::ostream& print_expr(std::ostream& out, Expression<NodeTrait>* expr);

// void append_args(Function* fun, Expression* expr);

// Compute the graph (all placeholder must be defined)
template<typename NodeTrait>
double full_eval(const Context<NodeTrait>& ctx, Expression<NodeTrait>* expr);

// free memory
template<typename NodeTrait>
void free(Expression<NodeTrait>* expr);

// Copy a tree
template<typename NodeTrait>
Expression<NodeTrait>* copy(Expression<NodeTrait>* expr, bool keep_borrowed = false);

template<typename NodeTrait>
std::ostream& operator<<(std::ostream& out, Expression<NodeTrait>* expr);

}



#undef log_trace
#define log_trace(...)

namespace kiwi {
    using BinaryOperator = std::function<double(double, double)>;
    using BinaryOperatorTable = std::unordered_map<std::string, BinaryOperator>;

    using UnaryOperator = std::function<double(double)> ;
    using UnaryOperatorTable = std::unordered_map<std::string, UnaryOperator>;

    BinaryOperatorTable& binary_table();
    UnaryOperatorTable& unary_table();

    BinaryOperator& binary_operator(const std::string& name);
    UnaryOperator& unary_operator(const std::string& name);

    template<typename NodeTrait>
    class TypePrinting : public StaticTypeVisitor<TypePrinting<NodeTrait>, NodeTrait, void, int> {
    public:
        TypePrinting(std::ostream& out) :
            out(out)
        {}

        static std::ostream& run(std::ostream& out, Type<NodeTrait>* expr, int idt = 1) {
            TypePrinting eval(out);

            if (!expr) {
                log_error("No Type to print");
                return out;
            }

            eval.traverse(expr, idt);
            return out;
        }

        void arrow(Arrow<NodeTrait>* x, int idt) {
            out << '(';

            for (std::size_t i = 0; i < x->args.size() - 1; ++i) {
                traverse(x->args[i], idt); out << ", ";
            }

            traverse(x->args.back(), idt);

            out << ") -> ";

            traverse(x->return_type, idt);
        }

        void builtin(Builtin<NodeTrait>* e, int) {
            out << NodeTrait::to_string(e->name);
        }

        void error(ErrorType<NodeTrait>* e, int) {

        }

        void unkown(Unknown<NodeTrait>* e, int) {

        }

        void record(Record<NodeTrait>* e, int) {

        }

        void error(ErrorNode<NodeTrait>* e, int) {

        }

    private:
        std::ostream& out;
    };


    template<typename NodeTrait>
    class Printing : public StaticVisitor<Printing<NodeTrait>, NodeTrait, void, int> {
    public:
        Printing(std::ostream& out) :
            out(out)
        {}

        static std::ostream& run(std::ostream& out, Expression<NodeTrait>* expr) {
            Printing eval(out);
            eval.traverse(expr, 1);
            return out;
        }

        void function(Function<NodeTrait>* x, int indentation) {
            int n = int(x->args_size()) - 1;
            out << "def " << x->name << "(";

            if (n >= 0) {
                for (int i = 0; i < n; ++i) {

                    out << x->arg(i);
                    if (x->type) {
                        out << ": ";
                        TypePrinting<NodeTrait>::run(out, x->type->arg(i), indentation);
                    }

                    out << ", ";
                }
                out << x->arg(n);
                if (x->type) {
                    out << ": ";
                    TypePrinting<NodeTrait>::run(out, x->type->arg(n), indentation);
                }
            }

            out << ")";

            if (x->type) {
                out << " -> ";
                TypePrinting<NodeTrait>::run(out, x->type->return_type, indentation);
            }

            out << ":\n" << std::string(indentation * 4, ' ');

            traverse(x->body, indentation + 1);
        }

        void function_call(FunctionCall<NodeTrait>* x, int indentation) {
            int n = x->args_size() - 1;

            out << x->name << "(";
            for (int i = 0; i < n; ++i) {
                traverse(x->arg(i), indentation);
                out << ", ";
            }

            traverse(x->arg(n), indentation);
            out << ")";
        }

        void unary_call(UnaryCall<NodeTrait>* x, int indentation) {
            if (!x->right)
                out << x->name;

            traverse(x->arg(0), indentation);

            if (x->right)
                out << x->name;
        }

        void binary_call(BinaryCall<NodeTrait>* x, int indentation) {
            out << "(";
            traverse(x->arg(0), indentation);
            out << " " << x->name << " ";
            traverse(x->arg(1), indentation);
            out << ")";
        }

        return_value borrow(Borrow<NodeTrait>* b, int indentation) {
            return traverse(b->expr, indentation);
        }

        void error(ErrorNode<NodeTrait>* e, int indentation) {
            out << "(error, m = " << e->message << ")";
        }

        void value(Value<NodeTrait>* x, int) {
            x->print(out);
            // out << x->as<f64>();
        }

        void placeholder(Placeholder<NodeTrait>* x, int) {
            out << x->name;
        }

        std::ostream& out;
    };

    /*/ Placeholders are inserted inside the function
    class AppendArgs: public StaticVisitor<AppendArgs, void>{
    public:
    AppendArgs(Function* fun):
    fun(fun)
    {}

    static void run(Function* fun, Expression* expr){
    AppendArgs eval(fun);
    return eval.traverse(expr);
    }

    void value(Value*){}
    void function(Function*){}

    void call(FunctionCall* x){
    for(int i = 0; i < x->args_size(); ++i){
    traverse(x->arg(i));
    }
    }

    void borrow(Borrow *b){

    }

    void placeholder(Placeholder* x){
    fun->args.push_back(x->name);
    }

    Function* fun;
    };*/


    template<typename NodeTrait>
    class FullEval : public StaticVisitor<FullEval<NodeTrait>, NodeTrait, double> {
    public:
        FullEval(const Context<NodeTrait>& ctx) :
            ctx(ctx)
        {}

        static double run(const Context<NodeTrait>& ctx, Expression<NodeTrait>* expr) {
            FullEval eval(ctx);
            return eval.traverse(expr);
        }

        // nothing to do...
        double function(Function<NodeTrait>*) {
            return 0;
        }

        double function_call(FunctionCall<NodeTrait>* x) {
            std::size_t count = ctx.count(x->name);

            if (count == 0) {
                log_error("Function does not exist");
                return 0;
            }

            Expression<NodeTrait>* efun = ctx.at(x->name);

            if (efun->tag != NodeTag::function) {
                log_error("Calling a non-function");
                return 0;
            }

            Function<NodeTrait>* fun = static_cast<Function<NodeTrait>*>(efun);

            if (fun->args_size() != x->args_size()) {
                log_error("argument size mismatch:",
                    fun->args_size(), " ", x->args_size());
                return 0;
            }
            else {
                // create eval context
                Context<NodeTrait> fun_ctx;

                for (int i = 0; i < fun->args_size(); ++i) {
                    fun_ctx[fun->arg(i)] = x->arg(i);
                    log_trace(fun->arg(i), " ", x->arg(i));
                }

                log_trace("Context built ", fun->body);
                return full_eval(fun_ctx, fun->body);
            }
        }

        double binary_call(BinaryCall<NodeTrait>* x) {
            double a = traverse(x->arg(0));
            double b = traverse(x->arg(1));

            return binary_operator(x->name)(a, b);
        }

        double unary_call(UnaryCall<NodeTrait>* x) {
            double a = traverse(x->arg(0));

            return unary_operator(x->name)(a);
        }

        return_value borrow(Borrow<NodeTrait>* b) {
            return traverse(b->expr);
        }

        double value(Value<NodeTrait>* x) {
            return x->as<f64>();
        }

        double placeholder(Placeholder<NodeTrait>* x) {
            return traverse(ctx.at(x->name));
        }

        return_value arrow(Arrow<NodeTrait>* x) {
            return 0;
        }

        return_value type(Type<NodeTrait>* x) {
            return 0;
        }

        return_value builtin(Builtin<NodeTrait>* e) {
            return 0;
        }

        return_value error(ErrorNode<NodeTrait>* e) {
            return 0;
        }

        const Context<NodeTrait>& ctx;
    };

    // TODO
    template<typename NodeTrait>
    class PartialEval : public StaticVisitor<PartialEval<NodeTrait>, NodeTrait, generic::Root<Expression<NodeTrait>>> {
    public:
        typedef typename generic::Root<Expression<NodeTrait>> Root;
        typedef typename generic::DummyRoot<Expression<NodeTrait>> DummyRoot;

        PartialEval(const Context<NodeTrait>& ctx) :
            ctx(ctx)
        {}

        static Root run(const Context<NodeTrait>& ctx, Expression<NodeTrait>* expr) {
            PartialEval eval(ctx);
            return eval.traverse(expr);
        }

        Expression<NodeTrait>* function(Function<NodeTrait>* x) {
            return x;
        }

        Expression<NodeTrait>* function_call(FunctionCall<NodeTrait>* x) {
            // we need to build a new context with the arguments

            // lookup the implementation

            // eval body with new context

            // return result
            return x;
        }

        Expression<NodeTrait>* unary_call(UnaryCall* x) {
            Expression<NodeTrait>* expr = traverse(x->expr);

            if (expr->tag == NodeTag::value) {
                Value<NodeTrait>* vexpr = static_cast<Value*>(expr);
                return Builder<DummyRoot, Tree>::value(
                    unary_operator(x->name)(vexpr->as<f64>()));
            }

            return Builder<DummyRoot, Tree>::unary_call(x->name, expr);
        }

        Expression<NodeTrait>* binary_call(BinaryCall<NodeTrait>* x) {
            Expression<NodeTrait>* lhs = traverse(x->lhs);
            Expression<NodeTrait>* rhs = traverse(x->rhs);

            if (lhs->tag == NodeTag::value && rhs->tag == NodeTag::value) {
                Value<NodeTrait>* vlhs = static_cast<Value<NodeTrait>*>(lhs);
                Value<NodeTrait>* vrhs = static_cast<Value<NodeTrait>*>(rhs);;
                return Builder<DummyRoot, Tree>::value(
                    binary_operator(x->name)(vlhs->as<f64>(), vrhs->as<f64>()));
            }

            return Builder<DummyRoot, Tree>::binary_call(x->name, lhs, rhs);
        }

        return_value borrow(Borrow<NodeTrait>* b) {
            return traverse(b->expr);
        }

        Expression<NodeTrait>* value(Value<NodeTrait>* x) {
            return x;
        }

        Expression<NodeTrait>* placeholder(Placeholder<NodeTrait>* x) {
            if (ctx.count(x->name) == 0)
                return x;

            return traverse(ctx.at(x->name));
        }

        return_value arrow(Arrow<NodeTrait>* x) {
            return x;
        }

        return_value type(Type<NodeTrait>* x) {
            return x;
        }

        return_value builtin(Builtin<NodeTrait>* e) {
            return e;
        }

        return_value error(ErrorNode<NodeTrait>* e) {
            return e;
        }

        const Context<NodeTrait>& ctx;
    };

    template<typename NodeTrait>
    class FreeMemory : public StaticVisitor<FreeMemory<NodeTrait>, NodeTrait, void> {
    public:
        FreeMemory() = default;

        static void run(Expression<NodeTrait>* expr) {
            if (expr == nullptr)
                return;

            FreeMemory<NodeTrait> eval;
            eval.traverse(expr);
            delete expr;
            return;
        }

        void function(Function<NodeTrait>* x) {
            traverse(x->body);
            delete x->body;
            return;
        }

        void general_call(Call<NodeTrait>& x) {
            for (int i = 0; i < x.args_size(); ++i) {
                traverse(x.arg(i));
                delete x.arg(i);
            }
            return;
        }

        void function_call(FunctionCall<NodeTrait>* x) {
            return general_call(*x);
        }

        void binary_call(BinaryCall<NodeTrait>* x) {
            return general_call(*x);
        }

        void unary_call(UnaryCall<NodeTrait>* x) {
            return general_call(*x);
        }

        void borrow(Borrow<NodeTrait>*) {
            // nothing to do. Borrow will be deleted by its parent
        }

        void value(Value<NodeTrait>* x) { return; }
        void placeholder(Placeholder<NodeTrait>* x) { return; }

        // TODO
        return_value arrow(Arrow<NodeTrait>* x) {
        }

        return_value type(Type<NodeTrait>* x) {
        }

        return_value builtin(Builtin<NodeTrait>* e) {
        }

        return_value error(ErrorNode<NodeTrait>* e) {
        }
    };

    template<typename NodeTrait>
    class Copy : public StaticVisitor<Copy<NodeTrait>, NodeTrait, Expression<NodeTrait>*> {
    public:
        typedef typename generic::Root<Expression<NodeTrait>> Root;
        typedef typename generic::DummyRoot<Expression<NodeTrait>> DummyRoot;

        Copy(bool keep_borrowed) :
            keep_borrowed(keep_borrowed)
        {}

        static Expression<NodeTrait>* run(Expression<NodeTrait>* expr, bool keep_borrowed = false) {
            if (expr == nullptr)
                return nullptr;

            Copy eval(keep_borrowed);
            return eval.traverse(expr);
        }

        Expression<NodeTrait>* function(Function<NodeTrait>* x) {
            return Builder<NodeTrait>::function(x->name, traverse(x->body));
        }

        Expression<NodeTrait>* function_call(FunctionCall<NodeTrait>* x) {
            std::vector<Expression<NodeTrait>*> new_args;

            std::transform(x->args.begin(), x->args.end(), // from -> to
                std::back_inserter(new_args),    // insert to
                [this](Expression<NodeTrait>* expr) { return traverse(expr); }); // copy each arguments

            return Builder<NodeTrait>::call(x->name, new_args);
        }

        Expression<NodeTrait>* unary_call(UnaryCall<NodeTrait>* x) {
            return Builder<NodeTrait>::unary_call(x->name, traverse(x->expr));
        }

        Expression<NodeTrait>* binary_call(BinaryCall<NodeTrait>* x) {
            return Builder<NodeTrait>::binary_call(x->name, traverse(x->lhs), traverse(x->rhs));
        }

        Expression<NodeTrait>* value(Value<NodeTrait>* x) {
            return Builder<NodeTrait>::value(x->as<f64>());
        }

        Expression<NodeTrait>* placeholder(Placeholder<NodeTrait>* x) {
            return Builder<NodeTrait>::placeholder(x->name);
        }

        Expression<NodeTrait>* borrow(Borrow<NodeTrait>* b) {
            if (keep_borrowed)
                return Builder<NodeTrait>::borrow(b->expr);

            return traverse(b->expr);
        }

        // TODO
        return_value arrow(Arrow<NodeTrait>* x) {
            return x;
        }

        return_value type(Type<NodeTrait>* x) {
            return x;
        }

        return_value builtin(Builtin<NodeTrait>* e) {
            return e;
        }

        return_value error(ErrorNode<NodeTrait>* e) {
            return e;
        }

        bool keep_borrowed;
    };


    template<typename NodeTrait>
    Expression<NodeTrait>* copy(Expression<NodeTrait>* expr, bool keep_borrowed) {
        return Copy<NodeTrait>::run(expr, keep_borrowed);
    }

    template<typename NodeTrait>
    std::ostream& print_expr(std::ostream& out, Expression<NodeTrait>* expr) {
        return Printing<NodeTrait>::run(out, expr);
    }

    template<typename NodeTrait>
    std::ostream& print_type(std::ostream& out, Type<NodeTrait>* expr) {
        return TypePrinting<NodeTrait>::run(out, expr);
    }

    template<typename NodeTrait>
    std::ostream& operator<<(std::ostream& out, Expression<NodeTrait>* expr) {
        return print_expr(out, expr);
    }

    template<typename NodeTrait>
    double full_eval(const Context<NodeTrait>& ctx, Expression<NodeTrait>* expr) {
        return FullEval<NodeTrait>::run(ctx, expr);
    }

    template<typename NodeTrait>
    void free(Expression<NodeTrait>* expr) {
        return FreeMemory<NodeTrait>::run(expr);
    }
    /*
    void append_args(Function* fun, Expression* expr){
    return AppendArgs::run(fun, expr);
    }*/

}
