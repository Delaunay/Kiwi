#ifndef KIWI_BINARY_CALL_DRAWABLE_NODE_HEADER
#define KIWI_BINARY_CALL_DRAWABLE_NODE_HEADER

#include "ExpressionDrawable.h"

namespace kiwi {

class BinaryCallDrawable : public ExpressionDrawable {
  public:
    BinaryCallDrawable(BinaryCall *bexpr) {
        Point pos = {0, 0};

        std::tie(std::ignore, pos) = insert_entity_after(make_string("("), pos);

        std::tie(lhs, pos) = insert_entity_after(ExpressionDrawable::make(bexpr->lhs), pos);

        std::tie(std::ignore, pos) = insert_entity_after(make_string(" "), pos);

        std::tie(fun, pos) = insert_entity_after(ExpressionDrawable::make(bexpr->fun), pos);

        std::tie(std::ignore, pos) = insert_entity_after(make_string(" "), pos);

        std::tie(rhs, pos) = insert_entity_after(ExpressionDrawable::make(bexpr->rhs), pos);

        std::tie(std::ignore, pos) = insert_entity_after(make_string(")"), pos);
    }

  private:
    Drawable *fun;
    Drawable *lhs;
    Drawable *rhs;
};

} // namespace kiwi

#endif
