#pragma once

#include <json11/json11.hpp>

#include "../Root.h"
#include "../Expression.h"
#include "../Visitor.h"
#include "../../Logging/Log.h"

namespace kiwi {
    // Functions
    // ------------------------------------------------------------------------
    using Json = json11::Json;

    template<typename NodeTrait>
    Json& expression_to_json(Expression<NodeTrait>* expr);

    template<typename NodeTrait>
    Json type_to_json(Expression<NodeTrait>* expr);

    // Implementation
    // ------------------------------------------------------------------------
    template<typename NodeTrait>
    class TypeToJson : public StaticTypeVisitor<TypeToJson<NodeTrait>, NodeTrait, Json, int> {
    public:
        static Json run(Type<NodeTrait>* expr, int idt = 1) {
            return TypeToJson().traverse(expr, idt);
        }

        Json arrow(Arrow<NodeTrait>* x, int idt) {
            return json11::Json();
        }

        Json builtin(Builtin<NodeTrait>* e, int) {
            return json11::Json();
        }

        Json unkown(Unknown<NodeTrait>* e, int) {
            return json11::Json();
        }

        Json record(Record<NodeTrait>* e, int) {
            return json11::Json();
        }

        Json error(ErrorNode<NodeTrait>* e, int) {
            return json11::Json();
        }
    };

    template<typename NodeTrait>
    class ExpressionToJson : public StaticVisitor<ExpressionToJson<NodeTrait>, NodeTrait, Json, int> {
    public:
        ExpressionToJson()
        {}

        static Json run(Expression<NodeTrait>* expr) {
            return ExpressionToJson().traverse(expr, 1);
        }

        Json function(Function<NodeTrait>* x, int indentation) {
            return json11::Json();
        }

        Json function_call(FunctionCall<NodeTrait>* x, int indentation) {
            return json11::Json();
        }

        Json unary_call(UnaryCall<NodeTrait>* x, int indentation) {
            return json11::Json();
        }

        Json binary_call(BinaryCall<NodeTrait>* x, int indentation) {
            return json11::Json();
        }

        Json borrow(Borrow<NodeTrait>* b, int indentation) {
            return traverse(b->expr, indentation);
        }

        Json error(ErrorNode<NodeTrait>* e, int indentation) {
            return json11::Json();
        }

        Json value(Value<NodeTrait>* x, int) {
            return json11::Json();
        }

        Json placeholder(Placeholder<NodeTrait>* x, int) {
            return json11::Json();
        }
    };

    template<typename NodeTrait>
    Json expression_to_json(Expression<NodeTrait>* expr) {
        return ExpressionToJson<NodeTrait>::run(out, expr);
    }

    template<typename NodeTrait>
    Json type_to_json(Type<NodeTrait>* expr) {
        return TypeToJson<NodeTrait>::run(out, expr);
    }
}