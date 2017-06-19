#pragma once
#include "Expression.h"
#include "Value.h"

namespace kiwi{

/*
 * Define our light weight AST
 */
template<typename NodeType>
struct AST{
    typedef NodeType Node;
    typedef typename generic::Call<NodeType> Call;

#define X(name, object) typedef generic::object<NodeType> object;
    KIWI_AST_NODES
#undef X
};

typedef AST<Expression> LightAST;

}

