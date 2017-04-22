#include "Visitor.h"
#include "TreeOperations.h"
#include "Builder.h"

namespace kiwi{

class Printing: public StaticVisitor<Printing, void>{
public:
    Printing(std::ostream& out):
        out(out)
    {}

    static void run(std::ostream& out, Expression* expr){
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

    static double run(const Context& ctx, Expression* expr){
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

    static void run(Expression* expr){
        if (expr == nullptr)
            return;

        FreeMemory eval;
        eval.traverse(expr);
        delete expr;
        return;
    }

    void add(Add* x){
        traverse(x->lhs);
        delete x->lhs;

        traverse(x->rhs);
        delete x->rhs;

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

    Expression* add(Add* x){
        return Builder<DummyRoot>::add(traverse(x->lhs),
                                       traverse(x->rhs));
    }

    Expression* value(Value* x){
        return Builder<DummyRoot>::value(x->value);
    }

    Expression* placeholder(Placeholder* x){
        return Builder<DummyRoot>::placeholder(x->name);
    }

    Expression* borrow(Borrow* b){
        if (keep_borrowed)
            return b;
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
