#ifndef KIWI_BUILTIN_DRAWABLE_HEADER
#define KIWI_BUILTIN_DRAWABLE_HEADER

#include "ExpressionDrawable.h"

namespace kiwi {

class BuiltinDrawable : public ExpressionDrawable {
  public:
    // Empty Function to be filled out by the user
    // filled with placeholders
    BuiltinDrawable(String const &str) {
        std::tie(_name, std::ignore) = insert_entity_after(make_builtin_name(str), {0, 0});
        log_info("(size: ", size().x, " x ", size().y, ")");
    }

    // Build the Drawable from a AST Function Node
    BuiltinDrawable(BuiltinType const *builtin, uint32 idt = 0) {
        std::tie(_name, std::ignore) =
            insert_entity_after(make_builtin_name(builtin->name), {0, 0});
    }

    String const drawable_name() override { return "BuiltinDrawable"; }

  private:
    Drawable *_name = nullptr;
};
} // namespace kiwi

#endif
