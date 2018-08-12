#ifndef KIWI_FUNCTION_DRAWABLE_HEADER
#define KIWI_FUNCTION_DRAWABLE_HEADER

#include "ExpressionDrawable.h"

namespace kiwi {

class FunctionDrawable : public ExpressionDrawable {
  public:
    // Empty Function to be filled out by the user
    // filled with placeholders
    FunctionDrawable() {}

    // Build the Drawable from a AST Function Node
    template <typename T> FunctionDrawable(Function<T> const *fun, uint32 idt = 0) {
        Point pos{0, 0};

        // Prototype
        std::tie(std::ignore, pos) = insert_entity_after(make_keyword("def "), pos);
        std::tie(name, pos)        = insert_entity_after(make_function_name(fun->name), pos);
        std::tie(std::ignore, pos) = insert_entity_after(make_string("("), pos);

        //
        i64 n = fun->args_size() - 1;

        if(n >= 0) {
            types = Array<Drawable *>(n + 1);
            args  = Array<Drawable *>(n + 1);

            for(size_t i = 0; i < n; ++i) {
                log_trace("argument ", i + 1);

                std::tie(args[i], pos) = insert_entity_after(make_argument_name(fun->arg(i)), pos);
                std::tie(std::ignore, pos) = insert_entity_after(make_string(": "), pos);
                std::tie(types[i], pos) =
                    insert_entity_after(ExpressionDrawable::make(fun->type->arg(i)), pos);
                std::tie(std::ignore, pos) = insert_entity_after(make_string(", "), pos);
            }

            std::tie(args[n], pos)     = insert_entity_after(make_argument_name(fun->arg(n)), pos);
            std::tie(std::ignore, pos) = insert_entity_after(make_string(": "), pos);

            std::tie(types[n], pos) =
                insert_entity_after(ExpressionDrawable::make(fun->type->arg(n)), pos);
        }

        std::tie(std::ignore, pos) = insert_entity_after(make_string(")"), pos);
        std::tie(std::ignore, pos) = insert_entity_after(make_string(style.arrow()), pos);

        std::tie(return_type, pos) =
            insert_entity_after(ExpressionDrawable::make(fun->type->return_type), pos);
        std::tie(std::ignore, pos) = insert_entity_after(make_string(": "), pos);

        // I could make both of those dummy Drawables to ease size computation when changes are
        // registered
        pos = new_line(pos);
        pos = indent(pos, idt + 1);

        std::tie(body, pos) = insert_entity_after(ExpressionDrawable::make(fun->body), pos);
    }

    String const drawable_name() override { return "FunctionDrawable"; }

  private:
    Drawable *name;
    Array<Drawable *> types;
    Array<Drawable *> args;
    Drawable *return_type;
    Drawable *body;
};

} // namespace kiwi

#endif
