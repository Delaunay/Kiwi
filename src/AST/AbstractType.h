#ifndef KIWI_AST_ABS_TYPE_NODE_HEADER
#define KIWI_AST_ABS_TYPE_NODE_HEADER

#include "../Types.h"
#include "Expression.h"
#include "Tag.h"

/*  Expression Representing Types
 *
 *  Because we want a dependent types we need something to bridge between expression and types
 *
 */
namespace kiwi {

class AbsType : public Expression {
  public:
    AbsType(NodeTag tag) : Expression(tag) {}
};

} // namespace kiwi

#endif
