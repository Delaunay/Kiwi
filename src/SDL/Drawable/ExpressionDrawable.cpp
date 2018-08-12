#include "ExpressionDrawable.h"
#include "BinaryCallDrawable.h"
#include "BuiltinDrawable.h"
#include "FunctionCallDrawable.h"
#include "FunctionDrawable.h"
#include "PlaceholderDrawable.h"
#include "UnaryCallDrawable.h"
#include "ValueDrawable.h"

namespace kiwi {
std::unique_ptr<ExpressionDrawable> ExpressionDrawable::make(Expression<LightExpression> *elem) {
    switch(elem->tag) {
    case NodeTag::function:
        return std::make_unique<FunctionDrawable>(static_cast<Function<LightExpression> *>(elem));
    case NodeTag::value:
        return std::make_unique<ValueDrawable>(static_cast<Value<LightExpression> *>(elem));
    case NodeTag::placeholder:
        return std::make_unique<PlaceholderDrawable>(
            static_cast<Placeholder<LightExpression> *>(elem));
    case NodeTag::binary_call:
        return std::make_unique<BinaryCallDrawable>(
            static_cast<BinaryCall<LightExpression> *>(elem));
    case NodeTag::unary_call:
        return std::make_unique<UnaryCallDrawable>(static_cast<UnaryCall<LightExpression> *>(elem));
    case NodeTag::function_call:
        return std::make_unique<FunctionCallDrawable>(
            static_cast<FunctionCall<LightExpression> *>(elem));
    case NodeTag::borrow:
    case NodeTag::match:
    case NodeTag::error:
    default:
        return std::make_unique<BuiltinDrawable>("Body");
    }
}

std::unique_ptr<ExpressionDrawable> ExpressionDrawable::make(Type<LightExpression> *elem) {
    switch(elem->tag) {
    case NodeTypeTag::builtin:
        return std::make_unique<BuiltinDrawable>(static_cast<Builtin<LightExpression> *>(elem));
    default:
        return std::make_unique<BuiltinDrawable>("Type");
    }
}
} // namespace kiwi
