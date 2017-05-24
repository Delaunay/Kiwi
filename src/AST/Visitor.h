#pragma once
#include "LightAST.h"

#define KIWI_UNREACHABLE()

namespace kiwi{

namespace generic{
/*
 * StaticVisitor does not rely on vtables and thus might be optimized better
 * as small functions could be inlined
 */
template<typename Visitor, typename Node, typename RetType, typename ...Args>
class StaticVisitor{
public:
    typedef RetType return_value;

    RetType traverse(Node* x, Args... args){
        switch(x->tag){
    #define X(name, object)\
        case NodeTag::name:{\
            object<Node>* exp = static_cast<object<Node>*>(x);\
            return static_cast<Visitor&>(*this).name(exp, args...);\
        }
        KIWI_AST_NODES
    #undef X
        default:
            KIWI_UNREACHABLE();
            return RetType();
        }
    }

#define X(name, object)\
    RetType name(object<Node>* x, Args... args){\
        log_warning("unimplemented default behavior");\
        return RetType();\
    }
    KIWI_AST_NODES
#undef X
};

#ifdef VTABLE_VISITOR
#   define KIWI_DV_INHERIT
#else
#   define KIWI_DV_INHERIT : public StaticVisitor<DynamicVisitor<Node>, Node, void>
#endif

/*
 * DynamicVisitor is a standard visitor. It can rely on Expression's vtables
 * if their usage is activated
 */
template<typename Node>
class DynamicVisitor KIWI_DV_INHERIT{
public:

    virtual ~DynamicVisitor() {}

    VTABLEV(
    void traverse(Node x){
        x->visit(this);
    })

#define X(name, object) virtual void name(object<Node>* x) = 0;
    KIWI_AST_NODES
#undef X
};
}

template<typename Visitor, typename RetType, typename ...Args>
class StaticVisitor: public generic::StaticVisitor<Visitor, Expression, RetType, Args...>{

};

}
