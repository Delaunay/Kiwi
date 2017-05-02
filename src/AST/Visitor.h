#pragma once
#include "Expression.h"

#define KIWI_UNREACHABLE()

namespace kiwi{

/*
 * StaticVisitor does not rely on vtables and thus might be optimized better
 * as small functions could be inlined
 */
template<typename Visitor, typename RetType>
class StaticVisitor{
public:
    RetType traverse(Expression* x){
        switch(x->tag){/*
        case NodeTag::add:{
            Add* exp = static_cast<Add*>(x);
            return static_cast<Visitor&>(*this).add(exp);
        }*/
        case NodeTag::function:{
            Function* exp = static_cast<Function*>(x);
            return static_cast<Visitor&>(*this).function(exp);
        }
        case NodeTag::call:{
            FunctionCall* exp = static_cast<FunctionCall*>(x);
            return static_cast<Visitor&>(*this).call(exp);
        }
        // leafs
        case NodeTag::value:{
            Value* exp = static_cast<Value*>(x);
            return static_cast<Visitor&>(*this).value(exp);
        }
        case NodeTag::placeholder:{
            Placeholder* exp = static_cast<Placeholder*>(x);
            return static_cast<Visitor&>(*this).placeholder(exp);
        }
        case NodeTag::borrow:{
            Borrow* exp = static_cast<Borrow*>(x);
            return static_cast<Visitor&>(*this).borrow(exp);
        }
        default:
            KIWI_UNREACHABLE();
            return RetType();
        }
    }

    // default behavior is: Borrows are ignored
    RetType borrow(Borrow* b){
        return traverse(b->expr);
    }
};

#ifdef VTABLE_VISITOR
#   define KIWI_DV_INHERIT
#else
#   define KIWI_DV_INHERIT : public StaticVisitor<DynamicVisitor, void>
#endif

/*
 * DynamicVisitor is a standard visitor. It can rely on Expression's vtables
 * if their usage is activated
 */
class DynamicVisitor KIWI_DV_INHERIT{
public:

    virtual ~DynamicVisitor() {}

    VTABLEV(
    void traverse(Expr x){
        x->visit(this);
    })

    //virtual void add(Add* x) = 0;
    virtual void function(Function* x) = 0;
    virtual void call(FunctionCall* x) = 0;

    virtual void value(Value* x) = 0;
    virtual void placeholder(Placeholder* x) = 0;
};

}
