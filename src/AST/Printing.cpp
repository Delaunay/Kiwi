#include "Visitor.h"
#include "Printing.h"

namespace kiwi{

class Printing: public StaticVisitor<Printing, void>{
public:
    Printing(std::ostream& out):
        out(out)
    {}

    static void run(std::ostream& out, Expr expr){
        Printing eval(out);
        return eval.traverse(expr);
    }

    void add(Add* x){
        traverse(x->lhs);
        out << " + ";
        traverse(x->rhs);
    }

    void value(Value* x){
        out << x->value;
    }

    void placeholder(Placeholder* x){
        out << x->name;
    }

    std::ostream& out;
};


class FullEval: public StaticVisitor<FullEval, double>{
public:
    FullEval(const Context& ctx):
        ctx(ctx)
    {}

    static double run(const Context& ctx, Expr expr){
        FullEval eval(ctx);
        return eval.traverse(expr);
    }

    double add(Add* x){
        return traverse(x->lhs) + traverse(x->rhs);
    }

    double value(Value* x){
        return x->value;
    }

    double placeholder(Placeholder* x){
        return ctx.at(x->name);
    }

    const Context& ctx;
};


class FreeMemory: public StaticVisitor<FreeMemory, void>{
public:
    FreeMemory() = default;

    static void run(Expr expr){
        FreeMemory eval;
        return eval.traverse(expr);
    }

    void add(Add* x){
        traverse(x->lhs);
        delete x->lhs;

        traverse(x->rhs);
        delete x->rhs;

        return;
    }

    void value      (Value* x)      {   return;}
    void placeholder(Placeholder* x){   return;}
};


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
