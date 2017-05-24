#pragma once
#include "Expression.h"
#include "Value.h"

namespace kiwi{

/*
 * Define our light weight AST
 */
#define X(name, object) typedef generic::object<Expression> object;
    KIWI_AST_NODES
#undef X

typedef generic::Call<Expression> Call;

}
