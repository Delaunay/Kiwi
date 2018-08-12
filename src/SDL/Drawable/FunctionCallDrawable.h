#ifndef KIWI_FUNCTION_CALL_DRAWABLE_NODE_HEADER
#define KIWI_FUNCTION_CALL_DRAWABLE_NODE_HEADER

#include "ExpressionDrawable.h"

namespace kiwi {

class FunctionCallDrawable : public ExpressionDrawable {
  public:
    FunctionCallDrawable(FunctionCall<LightExpression> *callexpr) : args(callexpr->args_size()) {
        Point pos          = {0, 0};
        std::tie(fun, pos) = insert_entity_after(ExpressionDrawable::make(callexpr->fun), pos);
        std::tie(std::ignore, pos) = insert_entity_after(make_string("("), pos);

        u64 size      = callexpr->args_size() - 1;
        Drawable *arg = nullptr;

        for(u64 i = 0; i < size; ++i) {
            std::tie(arg, pos) =
                insert_entity_after(ExpressionDrawable::make(callexpr->arg(i)), pos);
            args.push_back(arg);
            std::tie(std::ignore, pos) = insert_entity_after(make_string(", "), pos);
        }

        std::tie(std::ignore, pos) = insert_entity_after(make_string(")"), pos);
    }

  private:
    Array<Drawable *> args;
    Drawable *fun;
};

} // namespace kiwi

#endif
