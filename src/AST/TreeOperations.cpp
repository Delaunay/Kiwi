#include "Visitor.h"
#include "TreeOperations.h"
#include "Builder.h"

#include <iterator>
#include <algorithm>
#include <functional>

//#define KIWI_DEBUG
#include "../Debug.h"

namespace kiwi{

typedef std::function<double(double, double)> BinaryOperator;
typedef std::unordered_map<std::string,
    BinaryOperator> BinaryOperatorTable;

typedef std::function<double(double)> UnaryOperator;
typedef std::unordered_map<std::string,
    UnaryOperator> UnaryOperatorTable;

BinaryOperatorTable& binary_table(){
    static BinaryOperatorTable op = {
        {"+", [](double a, double b){ return a + b;}},
        {"-", [](double a, double b){ return a - b;}},
        {"*", [](double a, double b){ return a * b;}},
        {"/", [](double a, double b){ return a / b;}},
    };
    return op;
}

UnaryOperatorTable& unary_table(){
    static UnaryOperatorTable op = {
        {"ln",     [](double a){ return std::log(a);}},
        {"exp",    [](double a){ return std::exp(a);}},
        {"sqrt",   [](double a){ return std::sqrt(a);}},
        {"return", [](double a){ return a;}},
    };
    return op;
}

BinaryOperator& binary_operator(const std::string& name){
    return binary_table()[name];
}

UnaryOperator& unary_operator(const std::string& name){
    return unary_table()[name];
}


class Printing: public LightStaticVisitor<Printing, void, int>{
public:
    Printing(std::ostream& out):
        out(out)
    {}

    static std::ostream& run(std::ostream& out, Expression* expr){
        Printing eval(out);
        eval.traverse(expr, 1);
        return out;
    }

    void function(Function* x, int indentation){
        int n = int(x->args_size()) - 1;
        out << "def " << x->name << "(";

        if (n >= 0){
            for(int i = 0; i < n; ++i){
                out << x->arg(i) << ", ";
            }
            out << x->arg(n);
        }

        out << "):\n" << std::string(indentation * 4, ' ');
        traverse(x->body, indentation + 1);
    }

    void function_call(FunctionCall* x, int indentation){
        int n = x->args_size() - 1;

        out << x->name << "(";
        for(int i = 0; i < n; ++i){
            traverse(x->arg(i), indentation);
            out << ", ";
        }

        traverse(x->arg(n), indentation);
        out << ")";
    }

    void unary_call(UnaryCall* x, int indentation){
        if (!x->right)
            out << x->name;

        traverse(x->arg(0), indentation);

        if (x->right)
            out << x->name;
    }

    void binary_call(BinaryCall* x, int indentation){
        out << "(";
        traverse(x->arg(0), indentation);
        out << " " << x->name << " ";
        traverse(x->arg(1), indentation);
        out << ")";
    }

    return_value borrow(Borrow* b, int indentation){
        return traverse(b->expr, indentation);
    }

    void arrow(Arrow* x, int indentation){
        out << "(";

        int n = int(x->args.size()) - 1;
        for(int i = 0; i < n; ++i){
            traverse(x->arg(i), indentation);
            out << ", ";
        }
        traverse(x->arg(n), indentation);

        out << ") -> ";
        traverse(x->return_type, indentation);
    }

    void type(Type* x, int indentation){
        out << x->name;
    }

    void builtin(Builtin* e, int indentation){
        out << "(Builtin " << e->name << ")";
    }

    void error(ErrorNode* e, int indentation){
        out << "(error, m = " << e->message << ")";
    }

    void value(Value* x, int){
        x->print(out);
        // out << x->as<f64>();
    }

    void placeholder(Placeholder* x, int){
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


class FullEval: public LightStaticVisitor<FullEval, double>{
public:
    FullEval(const Context& ctx):
        ctx(ctx)
    {}

    static double run(const Context& ctx, Expression* expr){
        FullEval eval(ctx);
        return eval.traverse(expr);
    }

    // nothing to do...
    double function(Function*){
        return 0;
    }

    double function_call(FunctionCall* x){
        std::size_t count = ctx.count(x->name);

        if (count == 0){
            printd("Function does not exist");
            return 0;
        }

        Expression* efun = ctx.at(x->name);

        if (efun->tag != NodeTag::function){
            printd("Calling a non-function");
            return 0;
        }

        Function* fun = static_cast<Function*>(efun);

        if (fun->args_size() != x->args_size()){
            printd("argument size mismatch:" <<
                   fun->args_size()  << " " << x->args_size());
            return 0;
        } else {
            // create eval context
            Context fun_ctx;

            for(int i = 0; i < fun->args_size(); ++i){
                fun_ctx[fun->arg(i)] = x->arg(i);
                printd(fun->arg(i) << " "; print(std::cout, x->arg(i)); std::cout);
            }

            printd("Context built "; print(std::cout, fun->body); std::cout);
            return full_eval(fun_ctx, fun->body);
        }
    }

    double binary_call(BinaryCall* x){
        double a = traverse(x->arg(0));
        double b = traverse(x->arg(1));

        return binary_operator(x->name)(a, b);
    }

    double unary_call(UnaryCall* x){
        double a = traverse(x->arg(0));

        return unary_operator(x->name)(a);
    }

    return_value borrow(Borrow* b){
        return traverse(b->expr);
    }

    double value(Value* x){
        return x->as<f64>();
    }

    double placeholder(Placeholder* x){
        return traverse(ctx.at(x->name));
    }

    return_value arrow(Arrow* x){
        return 0;
    }

    return_value type(Type* x){
        return 0;
    }

    return_value builtin(Builtin* e){
        return 0;
    }

    return_value error(ErrorNode* e){
        return 0;
    }

    const Context& ctx;
};

// TODO
class PartialEval: public LightStaticVisitor<PartialEval, generic::Root<Expression>>{
public:
    typedef typename generic::Root<Expression> Root;
    typedef typename generic::DummyRoot<Expression> DummyRoot;

    PartialEval(const Context& ctx):
        ctx(ctx)
    {}

    static Root run(const Context& ctx, Expression* expr){
        PartialEval eval(ctx);
        return eval.traverse(expr);
    }

    Expression* function(Function* x){
        return x;
    }

    Expression* function_call(FunctionCall* x){
        // we need to build a new context with the arguments

        // lookup the implementation

        // eval body with new context

        // return result
        return x;
    }

    Expression* unary_call(UnaryCall* x){
        Expression* expr = traverse(x->expr);

        if (expr->tag == NodeTag::value){
            Value* vexpr = static_cast<Value*>(expr);
            return Builder<DummyRoot, Tree>::value(
                     unary_operator(x->name)(vexpr->as<f64>()));
        }

        return Builder<DummyRoot, Tree>::unary_call(x->name, expr);
    }

    Expression* binary_call(BinaryCall* x){
        Expression* lhs = traverse(x->lhs);
        Expression* rhs = traverse(x->rhs);

        if (lhs->tag == NodeTag::value && rhs->tag == NodeTag::value){
            Value* vlhs = static_cast<Value*>(lhs);
            Value* vrhs = static_cast<Value*>(rhs);;
            return Builder<DummyRoot, Tree>::value(
                     binary_operator(x->name)(vlhs->as<f64>(), vrhs->as<f64>()));
        }

        return Builder<DummyRoot, Tree>::binary_call(x->name, lhs, rhs);
    }

    return_value borrow(Borrow* b){
        return traverse(b->expr);
    }

    Expression* value(Value* x){
        return x;
    }

    Expression* placeholder(Placeholder* x){
        if (ctx.count(x->name) == 0)
            return x;

        return traverse(ctx.at(x->name));
    }

    return_value arrow(Arrow* x){
        return x;
    }

    return_value type(Type* x){
        return x;
    }

    return_value builtin(Builtin* e){
        return e;
    }

    return_value error(ErrorNode* e){
        return e;
    }

    const Context& ctx;
};


class FreeMemory: public LightStaticVisitor<FreeMemory, void>{
public:
    FreeMemory() = default;

    static void run(Expression* expr){
        if (expr == nullptr)
            return;

        FreeMemory eval;
        eval.traverse(expr);
        delete expr;
        return;
    }

    void function(Function* x){
        traverse(x->body);
        delete x->body;
        return;
    }

    void general_call(Call& x){
        for(int i = 0; i < x.args_size(); ++i){
            traverse(x.arg(i));
            delete x.arg(i);
        }
        return;
    }

    void function_call(FunctionCall* x){
        return general_call(*x);
    }

    void binary_call(BinaryCall* x){
        return general_call(*x);
    }

    void unary_call(UnaryCall* x){
        return general_call(*x);
    }

    void borrow(Borrow*){
        // nothing to do. Borrow will be deleted by its parent
    }

    void value      (Value* x)      {   return;}
    void placeholder(Placeholder* x){   return;}

    // TODO
    return_value arrow(Arrow* x){
    }

    return_value type(Type* x){
    }

    return_value builtin(Builtin* e){
    }

    return_value error(ErrorNode* e){
    }
};

class Copy: public LightStaticVisitor<Copy, Expression*>{
public:
    typedef typename generic::Root<Expression> Root;
    typedef typename generic::DummyRoot<Expression> DummyRoot;

    Copy(bool keep_borrowed):
        keep_borrowed(keep_borrowed)
    {}

    static Expression* run(Expression* expr, bool keep_borrowed=false){
        if (expr == nullptr)
            return nullptr;

        Copy eval(keep_borrowed);
        return eval.traverse(expr);
    }

    Expression* function(Function* x){
        return Builder<DummyRoot, Tree>::function(x->name, traverse(x->body));
    }

    Expression* function_call(FunctionCall* x){
        std::vector<Expression*> new_args;

        std::transform(x->args.begin(), x->args.end(), // from -> to
                       std::back_inserter(new_args),    // insert to
                       [this](Expression* expr){ return traverse(expr); }); // copy each arguments

        return Builder<DummyRoot, Tree>::call(x->name, new_args);
    }
    Expression* unary_call(UnaryCall* x){
        return Builder<DummyRoot, Tree>::unary_call(x->name, traverse(x->expr));
    }
    Expression* binary_call(BinaryCall* x){
        return Builder<DummyRoot, Tree>::binary_call(x->name, traverse(x->lhs), traverse(x->rhs));
    }

    Expression* value(Value* x){
        return Builder<DummyRoot, Tree>::value(x->as<f64>());
    }

    Expression* placeholder(Placeholder* x){
        return Builder<DummyRoot, Tree>::placeholder(x->name);
    }

    Expression* borrow(Borrow* b){
        if (keep_borrowed)
            return Builder<DummyRoot, Tree>::borrow(b->expr);

        return traverse(b->expr);
    }

    // TODO
    return_value arrow(Arrow* x){
        return x;
    }

    return_value type(Type* x){
        return x;
    }

    return_value builtin(Builtin* e){
        return e;
    }

    return_value error(ErrorNode* e){
        return e;
    }

    bool keep_borrowed;
};


Expression* copy(Expression* expr, bool keep_borrowed){
    return Copy::run(expr, keep_borrowed);
}

std::ostream& print_expr(std::ostream& out, Expression* expr){
    return Printing::run(out, expr);
}

double full_eval(const Context& ctx, Expression* expr){
    return FullEval::run(ctx, expr);
}

void free(Expression* expr){
    return FreeMemory::run(expr);
}
/*
void append_args(Function* fun, Expression* expr){
    return AppendArgs::run(fun, expr);
}*/



}
