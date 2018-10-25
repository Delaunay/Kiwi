#ifndef KIWI_AST_KEXPRESSION_HEADER_H_
#define KIWI_AST_KEXPRESSION_HEADER_H_

#include "Statement.h"
#include <memory>

namespace kiwi {

// Restore Value Semantic to Kiwi Expressions
// https://www.youtube.com/watch?v=QGcVXgEVMJg
// Probably not a good use of it since we are defining an actual tree
// it is not an incidental data structure
class KExpression {
  public:
    template <typename T, typename... Args>
    KExpression(Args &&... args) : _self(std::make_shared<T>(std::forward(args)...)) {}

    template <typename T> const T &as() const {}

  private:
    std::shared_ptr<const Statement *> _self;
};

} // namespace kiwi
#endif
