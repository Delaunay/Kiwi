#ifndef KIWI_AST_EXPRESSION_HEADER
#define KIWI_AST_EXPRESSION_HEADER

#include "Statement.h"

#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

/*
 *  Define basic kiwi building blocks.
 *
 *  Those building blocks should not be instantiated by the user.
 *  Indeed Builder::Add/Placeholder should be used instead as
 *  those enable us to implement basic optimizations such as constant
 *  folding
 *
 * Everything is a template because we are going to generate different ASTs
 * i.e RenderTree to render the AST on the screen and a Lighweight AST
 * for internal use
 */
namespace kiwi {

template <typename T> T access(T *ptr) {
    if(ptr == nullptr) {
    }
    return ptr;
}

// TODO: test typing system
// TODO: Precedence table <= Or should this be parsing stuff

class Expression : public Statement {
  public:
    Expression(NodeTag tag) : Statement(tag) {}
};

/* Function calls are split in 3
 *      - call1, call to an unaryOperator
 *      - call2, call to a binaryOperator
 *      - calln, call to a function
 */
class Call : public Expression {
  public:
  protected:
    Call(NodeTag tag, Expression *fun) : Expression(tag), fun(fun) {}

  public:
    size_t args_size() const;
    Expression *arg(u64 index) const;

  public:
    Expression *fun;
    Expression *type{nullptr};
};

/*
 *      target match {
 *          case pattern => expression
 *          case _ => default
 *      }
 *
 *  Has to be an Expression that evaluate to a Value (then == must be defined for that type)
 *  it is basically a more generic switch we do not provide real pattern matching there yet.
 */
class Match : public Expression {
  public:
    Match() : Expression(NodeTag::match) {}

    Expression *target;
    Array<Tuple<Expression *, Expression *>> branches;
    Expression *default_branch = nullptr;
};

class UnaryCall final : public Call {
  public:
    UnaryCall(Expression *fun, Expression *expr) : Call(NodeTag::unary_call, fun), expr(expr) {}

    u64 args_size() const { return 1; }

    Expression *arg(u64 index) const {
        switch(index) {
        case 0:
            return expr;
        default:
            return expr;
        }
    }

    Expression *expr;
    bool right = false; // operator is left/right associative
};

class BinaryCall final : public Call {
  public:
    BinaryCall(Expression *fun, Expression *lhs, Expression *rhs) :
        Call(NodeTag::binary_call, fun), lhs(lhs), rhs(rhs) {}

    u64 args_size() const { return 2; }

    Expression *arg(u64 index) const {
        switch(index) {
        case 0:
            return lhs;
        case 1:
            return rhs;
        default:
            return lhs;
        }
    }

    Expression *lhs;
    Expression *rhs;
};

class FunctionCall final : public Call {
  public:
    FunctionCall(Expression *fun, const Array<Expression *> &args) :
        Call(NodeTag::function_call, fun), args(args) {}

    u64 args_size() const { return args.size(); }

    Expression *arg(u64 index) const { return args[index]; }

    const Array<Expression *> args;
};

class Placeholder final : public Expression {
  public:
    Placeholder(String const &name) : Expression(NodeTag::placeholder), name(name) {}

    String name;
    Expression *type{nullptr};
};

class Block final : public Expression {
  public:
    Block() : Expression(NodeTag::block) {}

    Expression *type{nullptr};
    Array<Statement *> statements;
};

// An ErrorNode is used by the parser
// when a parsing error occur, that way the parsing
// can keep going without stopping.
// Which makes our compiler able to report as many errors
// as possible. Moreover the parser is used extensively
// in our IDE, we can't allow our parser to crash:
// we need to be able to represent incorrect programs
//
// Interative IDE
// --------------
// ErrorNode hold a string that is used by the parser to
// replace ErrorNode with the resulting Node
//
// Parsing Auto-Correct
// --------------------
// ErrorNode also holds an expected Node which used
// when the parser can determine the node that should be present
class ErrorNode final : public Expression {
  public:
    ErrorNode(String const &message, String const &code = "", Expression *partial = nullptr) :
        Expression(NodeTag::error_type), message(message), code(code), partial(partial) {}

    Expression *expected{nullptr}; // suggested node
    String message;                // error message
    String code;                   // code the parser should use to solve the issue
    Expression *partial{nullptr};  // partial parsed Node
};

inline Expression *Call::arg(u64 index) const {
    switch(Expression::tag) {
    case NodeTag::unary_call:
        return static_cast<UnaryCall const &>(*this).arg(index);
    case NodeTag::binary_call:
        return static_cast<BinaryCall const &>(*this).arg(index);
    case NodeTag::function_call:
        return static_cast<FunctionCall const &>(*this).arg(index);
    default:
        return nullptr;
    }
}

inline u64 Call::args_size() const {
    switch(Expression::tag) {
    case NodeTag::unary_call:
        return static_cast<UnaryCall const &>(*this).args_size();
    case NodeTag::binary_call:
        return static_cast<BinaryCall const &>(*this).args_size();
    case NodeTag::function_call:
        return static_cast<FunctionCall const &>(*this).args_size();
    default:
        return 0;
    }
}
} // namespace kiwi

#endif
