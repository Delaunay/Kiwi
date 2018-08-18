#pragma once

#include "../Logging/Log.h"

#include "Definition.h"
#include "Expression.h"
#include "Type.h"
#include "Value.h"

#define KIWI_UNREACHABLE()

namespace kiwi {
// typedef Expression<NodeTrait> Node;
// typedef typename Call<NodeTrait> Call;

/*
 * StaticVisitor does not rely on vtables and thus might be optimized better
 * as small functions could be inlined
 * /
template <typename Visitor, typename RetType, typename... Args> class StaticVisitor {
  public:
    typedef RetType return_value;

    RetType traverse(Node *x, Args... args) {
        switch(x->tag) {
#define X(name, object, v)                                                                         \
    case NodeTag::name: {                                                                          \
        log_trace(#name);                                                                          \
        object *exp = static_cast<object *>(x);                                                    \
        return static_cast<Visitor &>(*this).name(exp, args...);                                   \
    }
            KIWI_AST_NODES(X)
#undef X
        default:
            log_error("Calling Unreachable Realm (tag: ", int(x->tag), ")");
            KIWI_UNREACHABLE();
            return RetType();
        }
    }

#define X(name, object, v)                                                                         \
    RetType name(object *, Args...) {                                                              \
        log_warn("unimplemented default behavior");                                                \
        return RetType();                                                                          \
    }
    KIWI_AST_NODES(X)
#undef X
};*/

} // namespace kiwi
