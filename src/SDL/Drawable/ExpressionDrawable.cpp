#include "ExpressionDrawable.h"
#include "FunctionDrawable.h"
#include "BuiltinDrawable.h"

namespace kiwi {
    std::unique_ptr<ExpressionDrawable> ExpressionDrawable::make(Expression<LightExpression>* elem) {
        switch (elem->tag) {
        case NodeTag::function:
            return std::make_unique<FunctionDrawable>(static_cast<Function<LightExpression>*>(elem));
        default:
            return std::make_unique<BuiltinDrawable>("Body");
        }
    }

    std::unique_ptr<ExpressionDrawable> ExpressionDrawable::make(Type<LightExpression>* elem) {
        switch (elem->tag) {
            //case NodeTypeTag::builtin:
            //    return std::make_unique<FunctionDrawable>(static_cast<Function<ASTTrait>*>(elem));
        default:
            return std::make_unique<BuiltinDrawable>("Type");
        }
    }
}