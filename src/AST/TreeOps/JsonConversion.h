#ifndef KIWI_AST_JSON_HEADER
#define KIWI_AST_JSON_HEADER

#include <json11/json11.hpp>

#include "../../Logging/Log.h"
#include "../Expression.h"
#include "../Root.h"
#include "../Visitor.h"

namespace kiwi {
/*/ Functions
// ------------------------------------------------------------------------
using Json = json11::Json;

template <typename NodeTrait> Json &expression_to_json(Expression *expr);

template <typename NodeTrait> Json type_to_json(Expression *expr);

// Implementation
// ------------------------------------------------------------------------
class ExpressionToJson : public StaticVisitor<ExpressionToJson, Json, int> {
  public:
    ExpressionToJson() {}

    static Json run(Node *expr) { return ExpressionToJson().traverse(expr, 1); }

    Json function(FunctionDeclaration *x, int indentation) { return json11::Json(); }

    Json function_call(FunctionCall *x, int indentation) { return json11::Json(); }

    Json unary_call(UnaryCall *x, int indentation) { return json11::Json(); }

    Json binary_call(BinaryCall *x, int indentation) { return json11::Json(); }

    Json borrow(Borrow *b, int indentation) { return traverse(b->expr, indentation); }

    Json error(ErrorNode *e, int indentation) { return json11::Json(); }

    Json value(Value *x, int) { return json11::Json(); }

    Json placeholder(Placeholder *x, int) { return json11::Json(); }

    Json function_type(FunctionType *x, int idt) { return json11::Json(); }

    Json builtin(BuiltinType *e, int) { return json11::Json(); }

    Json unkown(UnknownType *e, int) { return json11::Json(); }

    Json struct_decl(StructDeclaration *e, int) { return json11::Json(); }

    Json union_decl(UnionDeclaration *e, int) { return json11::Json(); }

    Json error_type(ErrorNode *e, int) { return json11::Json(); }
};

Json expression_to_json(Node *expr) { return ExpressionToJson::run(expr); }
*/
} // namespace kiwi

#endif
