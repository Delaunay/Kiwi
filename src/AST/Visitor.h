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
    RetType traverse(Expr x){
        switch(x->tag){
        case NodeTag::add:{
            Add* exp = static_cast<Add*>(x);
            return static_cast<Visitor&>(*this).add(exp);
        }
        case NodeTag::value:{
            Value* exp = static_cast<Value*>(x);
            return static_cast<Visitor&>(*this).value(exp);
        }
        case NodeTag::placeholder:{
            Placeholder* exp = static_cast<Placeholder*>(x);
            return static_cast<Visitor&>(*this).placeholder(exp);
        }
        default:
            KIWI_UNREACHABLE();
            return RetType();
        }
    }
};

VTABLEV(class Dummy{};)

#ifdef VTABLE_VISITOR
#   define KIWI_DV_INHERIT Dummy
#else
#   define KIWI_DV_INHERIT StaticVisitor<DynamicVisitor, void>
#endif

/*
 * DynamicVisitor is a standard visitor. It can rely on vtables
 * if their usage is activated
 */
class DynamicVisitor: public KIWI_DV_INHERIT{
public:

    VTABLEV(
    void traverse(Expr x){
        x->visit(this);
    })

    virtual void add(Add* x) = 0;
    virtual void value(Value* x) = 0;
    virtual void placeholder(Placeholder* x) = 0;
};

}
