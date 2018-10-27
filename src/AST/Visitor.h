#pragma once

#include "../Logging/Log.h"

#include "Definition.h"
#include "Expression.h"
#include "Type.h"
#include "Value.h"

#define KIWI_UNREACHABLE()

namespace kiwi {

template <typename Return, typename... Args> class StatementVisitor {
  public:
    Return visit_statement(Statement *stmt, Args... args) {
        if(stmt == nullptr) {
            return nullptr_statement(args...);
        }
        if(stmt->is_def()) {
            log_info(to_string(stmt->tag));
            return visit_definition(static_cast<Definition *>(stmt), args...);
        }
        if(stmt->is_type()) {
            log_info(to_string(stmt->tag));
            return visit_type(static_cast<Type *>(stmt), args...);
        }
        if(stmt->is_expr()) {
            log_info(to_string(stmt->tag));
            return visit_expression(static_cast<Expression *>(stmt), args...);
        }
    }

    virtual Return visit_type(Type *type, Args... args)             = 0;
    virtual Return visit_definition(Definition *def, Args... args)  = 0;
    virtual Return visit_expression(Expression *expr, Args... args) = 0;

    virtual Return nullptr_statement(Args... args)                    = 0;
    virtual Return unhandled_statement(Statement *stmt, Args... args) = 0;

    virtual ~StatementVisitor() = default;
};

template <typename Return, typename... Args> class TypeVisitor {
  public:
    Return visit_type(Type *type, Args... args) {
        if(type == nullptr) {
            return nullptr_type(args...);
        }
        switch(type->tag) {
        case NodeTag::builtin_type: {
            return builtin_type(static_cast<BuiltinType *>(type), args...);
        }
        case NodeTag::function_type: {
            return function_type(static_cast<FunctionType *>(type), args...);
        }
        case NodeTag::union_type: {
            return union_type(static_cast<UnionType *>(type), args...);
        }
        case NodeTag::struct_type: {
            return struct_type(static_cast<StructType *>(type), args...);
        }
        default: { return unhandled_type(type, args...); }
        }
    }

    virtual Return builtin_type(BuiltinType *, Args... args)   = 0;
    virtual Return function_type(FunctionType *, Args... args) = 0;
    virtual Return union_type(UnionType *, Args... args)       = 0;
    virtual Return struct_type(StructType *, Args... args)     = 0;

    virtual Return nullptr_type(Args... args)           = 0;
    virtual Return unhandled_type(Type *, Args... args) = 0;

    virtual ~TypeVisitor() = default;
};

template <typename Return, typename... Args> class ExpressionVisitor {
  public:
    Return visit_expression(Expression *expr, Args... args) {
        if(expr == nullptr) {
            return nullptr_expression(args...);
        }

        switch(expr->tag) {
        case NodeTag::unary_call: {
            return unary_call(static_cast<UnaryCall *>(expr), args...);
        }
        case NodeTag::binary_call: {
            return binary_call(static_cast<BinaryCall *>(expr), args...);
        }
        case NodeTag::function_call: {
            return function_call(static_cast<FunctionCall *>(expr), args...);
        }
        case NodeTag::match: {
            return match(static_cast<Match *>(expr), args...);
        }
        case NodeTag::block: {
            return block(static_cast<Block *>(expr), args...);
        }
        // Leaves
        case NodeTag::placeholder: {
            return placeholder(static_cast<Placeholder *>(expr), args...);
        }
        case NodeTag::value: {
            return value(static_cast<Value *>(expr), args...);
        }
        default: { return unhandled_expression(expr, args...); }
        }
    }

    virtual Return unary_call(UnaryCall *, Args... args)       = 0;
    virtual Return binary_call(BinaryCall *, Args... args)     = 0;
    virtual Return function_call(FunctionCall *, Args... args) = 0;
    virtual Return match(Match *, Args... args)                = 0;
    virtual Return block(Block *, Args... args)                = 0;
    virtual Return placeholder(Placeholder *, Args... args)    = 0;
    virtual Return value(Value *, Args... args)                = 0;

    virtual Return nullptr_expression(Args... args)                 = 0;
    virtual Return unhandled_expression(Expression *, Args... args) = 0;

    virtual ~ExpressionVisitor() = default;
};

template <typename Return, typename... Args> class DefinitionVisitor {
  public:
    Return visit_definition(Definition *def, Args... args) {
        if(def == nullptr) {
            return nullptr_definition(args...);
        }

        switch(def->tag) {
        case NodeTag::function_def: {
            return function_def(static_cast<FunctionDefinition *>(def), args...);
        }
        case NodeTag::macro_def: {
            return macro_def(static_cast<MacroDefinition *>(def), args...);
        }
        case NodeTag::struct_def: {
            return struct_def(static_cast<StructDefinition *>(def), args...);
        }
        case NodeTag::union_def: {
            return union_def(static_cast<UnionDefinition *>(def), args...);
        }
        default: { return unhandled_definition(def, args...); }
        }
    }

    virtual Return function_def(FunctionDefinition *, Args... args) = 0;
    virtual Return macro_def(MacroDefinition *, Args... args)       = 0;
    virtual Return struct_def(StructDefinition *, Args... args)     = 0;
    virtual Return union_def(UnionDefinition *, Args... args)       = 0;

    virtual Return nullptr_definition(Args... args)                 = 0;
    virtual Return unhandled_definition(Definition *, Args... args) = 0;

    virtual ~DefinitionVisitor() = default;
};

} // namespace kiwi
