#include "Visitor.h"
#include "TreeOperations.h"
#include "Builder.h"

#include <functional>

//#define KIWI_DEBUG
#include "../Debug.h"

namespace kiwi{

class Printing: public StaticVisitor<Printing, void, int>{
public:
    Printing(std::ostream& out):
        out(out)
    {}

    static void run(std::ostream& out, Expression* expr){
        Printing eval(out);
        return eval.traverse(expr, 1);
    }

    /*
    void add(Add* x){
        traverse(x->lhs);
        out << " + ";
        traverse(x->rhs);
    }*/

    void function(Function* x, int indentation){
        int n = x->args_size() - 1;
        out << "def " << x->name << "(";

        if (n > 0){
            for(int i = 0; i < n; ++i){
                out << x->arg(i) << ", ";
            }
            // last
            out << x->arg(n);
        }

        out << "):\n" << std::string(indentation * 4, ' ');

        traverse(x->body, indentation + 1);
    }

    void call(FunctionCall* x, int indentation){
        std::size_t n = x->args_size() - 1;

        out << x->name << "(";
        for(int i = 0; i < n; ++i){
            traverse(x->arg(i), indentation);
            out << ", ";
        }

        // last
        traverse(x->arg(n), indentation);
        out << ")";
    }

    void value(Value* x, int){
        out << x->value;
    }

    void placeholder(Placeholder* x, int){
        out << x->name;
    }

    std::ostream& out;
};


class FullEval: public StaticVisitor<FullEval, double>{
public:
    FullEval(const Context& ctx):
        ctx(ctx)
    {}

    static double run(const Context& ctx, Expression* expr){
        FullEval eval(ctx);
        return eval.traverse(expr);
    }

    /*
    double add(Add* x){
        return traverse(x->lhs) + traverse(x->rhs);
    }*/

    // nothing to do...
    double function(Function*){
        return 0;
    }

    double call(FunctionCall* x){
        int count = ctx.count(x->name);
        if (count == 1){
            Expression* efun = ctx.at(x->name);

            if (efun->tag != NodeTag::function){
                printd("Calling a non-function");
            }

            Function* fun = static_cast<Function*>(efun);

            if (fun->args_size() == x->args_size()){
                printd("argument size match");

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

        // This is a temporary hack
        if (x->args_size() != 2)
            return 0;

        static std::unordered_map<std::string, std::function<double(double, double)>> op ={
            {"+", [](double a, double b){ return a + b;}},
            {"-", [](double a, double b){ return a - b;}},
            {"*", [](double a, double b){ return a * b;}},
            {"/", [](double a, double b){ return a / b;}},
        };

        double a = traverse(x->arg(0));
        double b = traverse(x->arg(1));
        return op[x->name](a, b);


        // we need to build a new context with the arguments

        // lookup the implementation

        // eval body with new context

        // return result
    }

    double value(Value* x){
        return x->value;
    }

    double placeholder(Placeholder* x){
        return traverse(ctx.at(x->name));
    }

    const Context& ctx;
};

class PartialEval: public StaticVisitor<PartialEval, Root>{
public:
    PartialEval(const Context& ctx):
        ctx(ctx)
    {}

    static Root run(const Context& ctx, Expression* expr){
        PartialEval eval(ctx);
        return eval.traverse(expr);
    }

    /*
    Expression* add(Add* x){
        Expression* lhs = traverse(x->lhs);
        Expression* rhs = traverse(x->rhs);

        if (lhs->tag == NodeTag::value && rhs->tag == NodeTag::value){
            Value* vlhs = static_cast<Value*>(lhs);
            Value* vrhs = static_cast<Value*>(rhs);;
            return Builder<DummyRoot>::value(vlhs->value + vrhs->value);
        }

        return Builder<DummyRoot>::add(lhs, rhs);
    }*/

    Expression* function(Function* x){
        return x;
    }

    Expression* call(FunctionCall* x){
        // we need to build a new context with the arguments

        // lookup the implementation

        // eval body with new context

        // return result
        return x;
    }

    Expression* value(Value* x){
        return x;
    }

    Expression* placeholder(Placeholder* x){
        if (ctx.count(x->name) == 0)
            return x;

        return traverse(ctx.at(x->name));
    }

    const Context& ctx;
};


class FreeMemory: public StaticVisitor<FreeMemory, void>{
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

    void call(FunctionCall* x){
        for(int i = 0; i < x->args_size(); ++i){
            traverse(x->arg(i));
            delete x->arg(i);
        }
        return;
    }

    void borrow(Borrow*){
        // nothing to do. Borrow will be deleted by its parent
    }

    void value      (Value* x)      {   return;}
    void placeholder(Placeholder* x){   return;}
};

class Copy: public StaticVisitor<Copy, Expression*>{
public:
    Copy(bool keep_borrowed):
        keep_borrowed(keep_borrowed)
    {}

    static Expression* run(Expression* expr, bool keep_borrowed=false){
        if (expr == nullptr)
            return nullptr;

        Copy eval(keep_borrowed);
        return eval.traverse(expr);
    }

    /*
    Expression* add(Add* x){
        return Builder<DummyRoot>::add(traverse(x->lhs),
                                       traverse(x->rhs));
    }*/

    Expression* function(Function* x){
        return Builder<DummyRoot>::function(x->name, traverse(x->body));
    }

    Expression* call(FunctionCall* x){
        return Builder<DummyRoot>::call(x->name, x->args);
    }

    Expression* value(Value* x){
        return Builder<DummyRoot>::value(x->value);
    }

    Expression* placeholder(Placeholder* x){
        return Builder<DummyRoot>::placeholder(x->name);
    }

    Expression* borrow(Borrow* b){
        if (keep_borrowed)
            return Builder<DummyRoot>::borrow(b->expr);

        return traverse(b->expr);
    }

    bool keep_borrowed;
};


Expression* copy(Expression* expr, bool keep_borrowed){
    return Copy::run(expr, keep_borrowed);
}

void print(std::ostream& out, Expression* expr){
    return Printing::run(out, expr);
}

double full_eval(const Context& ctx, Expression* expr){
    return FullEval::run(ctx, expr);
}

void free(Expression* expr){
    return FreeMemory::run(expr);
}

}
