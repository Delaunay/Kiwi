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

    /*
    void add(Add* x){
        traverse(x->lhs);
        out << " + ";
        traverse(x->rhs);
    }*/

    void unary(UnaryFunction* x){
        out << x->name << "(";
        traverse(x->expr);
        out << ")";
    }

    void binary(BinaryFunction* x){
        traverse(x->lhs);
        out << " " << x->name << " ";
        traverse(x->rhs);
    }

    void nnary(NnaryFunction* x){
        out << x->name << "(";
        for(int i = 0; i < x->args_size() - 1; ++i){
            traverse(x->arg(i));
            out << ", ";
        }

        // last
        traverse(x->arg(x->args_size() - 1));
        out << ")";
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

    /*
    double add(Add* x){
        return traverse(x->lhs) + traverse(x->rhs);
    }*/

    double unary(UnaryFunction* x){
        return traverse(x->expr);
    }

    double binary(BinaryFunction* x){
        return traverse(x->lhs) + traverse(x->rhs);
    }

    double nnary(NnaryFunction* x){
        return 0;
    }

    double value(Value* x){
        return x->value;
    }

    double placeholder(Placeholder* x){
        return ctx.at(x->name);
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

    Expression* binary(BinaryFunction* x){
        Expression* lhs = traverse(x->lhs);
        Expression* rhs = traverse(x->rhs);

        if (lhs->tag == NodeTag::value && rhs->tag == NodeTag::value){
            Value* vlhs = static_cast<Value*>(lhs);
            Value* vrhs = static_cast<Value*>(rhs);;
            return Builder<DummyRoot>::value(vlhs->value + vrhs->value);
        }

        return Builder<DummyRoot>::binary(x->name, lhs, rhs);
    }

    Expression* unary(UnaryFunction* x){
        return traverse(x->expr);
    }

    Expression* nnary(NnaryFunction* x){
        return Builder<DummyRoot>::value(0);
    }

    Expression* value(Value* x){
        return x;
    }

    Expression* placeholder(Placeholder* x){
        if (ctx.count(x->name) == 0)
            return x;

        return Builder<DummyRoot>::value(ctx.at(x->name));
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

    void unary(UnaryFunction* x){
        traverse(x->expr);
        delete x->expr;
        return;
    }

    void binary(BinaryFunction* x){
        traverse(x->lhs);
        delete x->lhs;

        traverse(x->rhs);
        delete x->rhs;

        return;
    }

    void nnary(NnaryFunction* x){
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

    Expression* unary(UnaryFunction* x){
        return Builder<DummyRoot>::unary(x->name, traverse(x->expr));
    }

    Expression* binary(BinaryFunction* x){
        return Builder<DummyRoot>::binary(x->name,
                                          traverse(x->lhs),
                                          traverse(x->rhs));
    }

    Expression* nnary(NnaryFunction* x){
        NnaryFunction* fun = static_cast<NnaryFunction*>(Builder<DummyRoot>::nnary(x->name).get());

        for(int i = 0; i < x->args_size(); ++i){
            fun->args.push_back(traverse(x->arg(i)));
        }

        fun->body = traverse(x->body);

        return fun;
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
