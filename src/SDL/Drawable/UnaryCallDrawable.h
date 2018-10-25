#ifndef KIWI_UNARY_CALL_DRAWABLE_NODE_HEADER
#define KIWI_UNARY_CALL_DRAWABLE_NODE_HEADER

#include "ExpressionDrawable.h"

namespace kiwi {

class UnaryCallDrawable : public ExpressionDrawable {
  public:
    UnaryCallDrawable(UnaryCall *uexpr) {
        Point pos = {0, 0};

        if(!uexpr->right) {
            std::tie(fun, pos) = insert_entity_after(ExpressionDrawable::make(uexpr->fun), pos);
            std::tie(std::ignore, pos) = insert_entity_after(make_string(" "), pos);
        }

        std::tie(expr, pos) = insert_entity_after(ExpressionDrawable::make(uexpr->expr), pos);

        if(uexpr->right) {
            std::tie(std::ignore, pos) = insert_entity_after(make_string(" "), pos);
            std::tie(fun, pos) = insert_entity_after(ExpressionDrawable::make(uexpr->fun), pos);
        }
    }

  private:
    Drawable *expr;
    Drawable *fun;
};

} // namespace kiwi

#endif
