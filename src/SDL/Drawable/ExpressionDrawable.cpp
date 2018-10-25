#include "../../AST/Definition.h"

#include "BinaryCallDrawable.h"
#include "BuiltinDrawable.h"
#include "ExpressionDrawable.h"
#include "FunctionCallDrawable.h"
#include "FunctionDrawable.h"
#include "PlaceholderDrawable.h"
#include "UnaryCallDrawable.h"
#include "ValueDrawable.h"

namespace kiwi {
std::unique_ptr<ExpressionDrawable> ExpressionDrawable::make(Definition *elem) {
    switch(elem->tag) {
    case NodeTag::function_def:
        return std::make_unique<FunctionDrawable>(static_cast<FunctionDefinition *>(elem));
    default:
        return std::make_unique<BuiltinDrawable>("Body");
    }
}

std::unique_ptr<ExpressionDrawable> ExpressionDrawable::make(Expression *elem) {
    switch(elem->tag) {
    case NodeTag::value:
        return std::make_unique<ValueDrawable>(static_cast<Value *>(elem));
    case NodeTag::placeholder:
        return std::make_unique<PlaceholderDrawable>(static_cast<Placeholder *>(elem));
    case NodeTag::binary_call:
        return std::make_unique<BinaryCallDrawable>(static_cast<BinaryCall *>(elem));
    case NodeTag::unary_call:
        return std::make_unique<UnaryCallDrawable>(static_cast<UnaryCall *>(elem));
    case NodeTag::function_call:
        return std::make_unique<FunctionCallDrawable>(static_cast<FunctionCall *>(elem));
    case NodeTag::borrow:
    case NodeTag::match:
    case NodeTag::error_type:
    default:
        return std::make_unique<BuiltinDrawable>("Body");
    }
}

std::unique_ptr<ExpressionDrawable> ExpressionDrawable::make(Type *elem) {
    switch(elem->tag) {
    case NodeTag::builtin_type:
        return std::make_unique<BuiltinDrawable>(static_cast<BuiltinType *>(elem));
    default:
        return std::make_unique<BuiltinDrawable>("Type");
    }
}
} // namespace kiwi
