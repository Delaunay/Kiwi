#ifndef KIWI_AST_TYPE_NODE_HEADER
#define KIWI_AST_TYPE_NODE_HEADER

#include "Statement.h"

/*  Expression Representing Types
 *
 *  Because we want a dependent types we need something to bridge between expression and types
 *
 */
namespace kiwi {

class Type : public Statement {
  public:
    Type(NodeTag tag) : Statement(tag) {}
};

class FunctionType : public Type {
  public:
    FunctionType() : Type(NodeTag::function_type) {}

    FunctionType(const Array<Type *> &args, Type *return_type) :
        Type(NodeTag::function_type), args(args), return_type(return_type) {}

    size_t args_size() { return args.size(); }
    Type *arg(size_t i) { return args[i]; }

    Array<Type *> args;
    Type *return_type{nullptr};
};

//  Builtin Types
//  Mainly used to represent int/float/char/...
class BuiltinType : public Type {
  public:
    BuiltinType(String const &name) : Type(NodeTag::builtin_type), name(name) {}

    const String name;
};

//
//  A type that should be resolved later some type X
class UnknownType : public Type {
  public:
    UnknownType() : Type(NodeTag::unknown_type) {}

    bool is_resolved() { return resolved_type; }

    Type *resolved_type{nullptr};
};

// Random Error
class ErrorType : public Type {
  public:
    ErrorType() : Type(NodeTag::error_type) {}

    String message;
};
} // namespace kiwi

#endif
