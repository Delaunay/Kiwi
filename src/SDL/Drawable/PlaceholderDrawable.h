#ifndef KIWI_PLACEHOLDER_DRAWABLE_NODE_HEADER
#define KIWI_PLACEHOLDER_DRAWABLE_NODE_HEADER

#include "ExpressionDrawable.h"

namespace kiwi {

class PlaceholderDrawable : public ExpressionDrawable {
  public:
    PlaceholderDrawable(Placeholder *x) {

        Point pos = {0, 0};
        std::tie(_name, pos) =
            insert_entity_after(ExpressionDrawable::make_placeholder_name(x->name), pos);
    }

  private:
    Drawable *_name;
    Drawable *_type;
};

} // namespace kiwi

#endif
