#pragma once


#include "LightAST.h"
#include "../Logging/Log.h"

#define KIWI_UNREACHABLE()

namespace kiwi{
    //typedef Expression<NodeTrait> Node;
    //typedef typename Call<NodeTrait> Call;

    /*
    #define X(name, object) typedef object<NodeTrait> object;
    KIWI_AST_NODES
    #undef X*/

/*
 * StaticVisitor does not rely on vtables and thus might be optimized better
 * as small functions could be inlined
 */
template<typename Visitor, typename NodeTrait, typename RetType, typename ...Args>
class StaticVisitor{
public:
    typedef RetType return_value;

    RetType traverse(Expression<NodeTrait>* x, Args... args){
        switch(x->tag){
    #define X(name, object)\
        case NodeTag::name:{\
            log_trace(#name);\
            object<NodeTrait>* exp = static_cast<object<NodeTrait>*>(x);\
            return static_cast<Visitor&>(*this).name(exp, args...);\
        }
        KIWI_AST_NODES
    #undef X
        default:
            log_error("Calling Unreachable Realm (tag: ", int(x->tag), ")");
            KIWI_UNREACHABLE();
            return RetType();
        }
    }

#define X(name, object)\
    RetType name(object<NodeTrait>* x, Args... args){\
        log_warn("unimplemented default behavior");\
        return RetType();\
    }
    KIWI_AST_NODES
#undef X
};

template<typename Visitor, typename NodeTrait, typename RetType, typename ...Args>
class StaticTypeVisitor {
public:
    typedef RetType return_value;
    // --------------------------------------------------------------------------------
    //      Type Dispatcher
    // 
    RetType traverse(Type<NodeTrait>* x, Args... args) {
        switch (x->tag) {
    #define X(name, object)\
        case NodeTypeTag::name:{\
            log_trace(#name);\
            object<NodeTrait>* exp = static_cast<object<NodeTrait>*>(x);\
            return static_cast<Visitor&>(*this).name(exp, args...);\
        }
            KIWI_TYPE_NODES
    #undef X
        default:
            log_error("Calling Unreachable Realm (type_tag: ", int(x->tag), ")");
            KIWI_UNREACHABLE();
            return RetType();
        }
    }

#define X(name, object)\
    RetType name(object<NodeTrait>* x, Args... args){\
        log_warn("unimplemented default behavior");\
        return RetType();\
    }
    KIWI_TYPE_NODES
#undef X
};


#ifdef VTABLE_VISITOR
#   define KIWI_DV_INHERIT
#else
#   define KIWI_DV_INHERIT : public StaticVisitor<DynamicVisitor<NodeTrait>, NodeTrait, void>
#endif

/*
 * DynamicVisitor is a standard visitor. It can rely on Expression's vtables
 * if their usage is activated
 */
template<typename NodeTrait>
class DynamicVisitor KIWI_DV_INHERIT{
public:
    //typedef Expression<NodeTrait> Node;
    //typedef typename Call<NodeTrait> Call;

    /*
    #define X(name, object) typedef generic::object<NodeTrait> object;
        KIWI_AST_NODES
    #undef X
    */

    virtual ~DynamicVisitor() {}

    VTABLEV(
    void traverse(Expression<NodeTrait> x){
        x->visit(this);
    })

#define X(name, object) virtual void name(Expression<NodeTrait>* x) = 0;
    KIWI_AST_NODES
#undef X
};

/*
template<typename Visitor, typename RetType, typename ...Args>
class LightStaticVisitor: public StaticVisitor<Visitor, LightAST, RetType, Args...>{
};*/

}
