#ifndef KIWI_VALUE_DRAWABLE_NODE_HEADER
#define KIWI_VALUE_DRAWABLE_NODE_HEADER

#include "ExpressionDrawable.h"

namespace kiwi {

class ValueDrawable : public ExpressionDrawable {
  public:
    ValueDrawable(Value<LightExpression> *value) {
        f64 data = value->template as<f64>();
        _value   = insert_entity(std::make_unique<StringDrawable>(std::to_string(data)));
    }

  private:
    Drawable *_value;
};

} // namespace kiwi

#endif
