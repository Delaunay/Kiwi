#pragma once

#include <iostream>
#include <string>
#include <vector>
//#include <memory>

//#define LINK_TO_PARENT
//#define VTABLE_VISITOR

// Shall we enable children to have a reference to
// their parents
#ifdef LINK_TO_PARENT
#   define PARENT(X) X
#else
#   define PARENT(X)
#endif

// Shall we use a vtable
#ifdef VTABLE_VISITOR
#   define VTABLEV(X) X
#   define STATICV(X)
#else
#   define VTABLEV(X)
#   define STATICV(X) X
#endif

/*
 *  Define basic kiwi building blocks.
 *
 *  Those building blocks should not be instantiated by the user.
 *  Indeed Builder::Add/Placeholder should be used instead as
 *  those enable us to implement basic optimizations such as constant
 *  folding
 */
namespace kiwi{

enum class NodeTag{
    value,
    placeholder,
    // add,

    // Functions
    unary,      // 1 argument
    binary,     // 2 arguments
    nnary,      // n arguments

    borrow
};

class Expression{
public:
    Expression(NodeTag tag):
        tag(tag)
    {}

    VTABLEV(virtual void visit(class DynamicVisitor* v) = 0;)

    NodeTag tag;
    PARENT(Expression* parent = nullptr;)
};

//typedef std::unique_ptr<Expression> Expr;
//typedef Expression* Expr;

class Function: public Expression{
public:
    std::string name;

    // Cant create a function, use Unary/Binary/Nnary instead
protected:
    Function(const std::string& op, NodeTag tag):
        Expression(tag), name(op)
    {}
public:

    std::size_t args_size() const;
    Expression* arg(std::size_t index);
};

class UnaryFunction: public Function{
public:
    Expression* expr;

    UnaryFunction(const std::string& op, Expression* expr):
        Function(op, NodeTag::unary), expr(expr)
    {}

    VTABLEV(void visit(class DynamicVisitor* v) override;)

    std::size_t args_size() const { return 1;}
    Expression* arg(std::size_t index) {
        switch(index){
        case 0: return expr;
        default: return expr;
        }
    }
};


class BinaryFunction: public Function{
public:
    Expression* lhs;
    Expression* rhs;

    BinaryFunction(const std::string& op, Expression* lhs, Expression* rhs):
        Function(op, NodeTag::binary), lhs(lhs), rhs(rhs)
    {}

    VTABLEV(void visit(class DynamicVisitor* v) override;)

    std::size_t args_size() const { return 2;}
    Expression* arg(std::size_t index) {
        switch(index){
        case 0: return lhs;
        case 1: return rhs;
        default: return lhs;
        }
    }
};

class NnaryFunction: public Function{
public:
    std::vector<Expression*> args;
    Expression*              body;

    NnaryFunction(const std::string& op):
        Function(op, NodeTag::nnary)
    {}

    VTABLEV(void visit(class DynamicVisitor* v) override;)

    std::size_t args_size() const { return args.size();}
    Expression* arg(std::size_t index) { return args[index]; }
};

/*
class Add: public Expression{
public:
    Add(Expression* lhs, Expression* rhs):
        Expression(NodeTag::add), lhs(lhs), rhs(rhs)
    {}

    VTABLEV(void visit(class DynamicVisitor* v) override;)

    Expression* lhs;
    Expression* rhs;
};*/

class Value: public Expression{
public:
    Value(double value):
        Expression(NodeTag::value), value(value)
    {}

    VTABLEV(void visit(class DynamicVisitor* v) override;)

    double value;
};

class Placeholder: public Expression{
public:
    Placeholder(const std::string name):
        Expression(NodeTag::placeholder), name(name)
    {}

    VTABLEV(void visit(class DynamicVisitor* v) override;)

    std::string name;
};

/* Borrow is a dummy Node which only exists to help
 * memory management. A Borrowed node is a node owned
 * by another tree. i.e we don't need to free it
 *
 * Borrow is mostly during meta programming phase
 * when the user generates code from a programm.
 * Borrow allow us to reuse the original program subtree
 * instead of copying it
 */
class Borrow: public Expression{
public:
    Borrow(Expression* expr):
        Expression(NodeTag::borrow), expr(expr)
    {}

    Expression* expr;
};


}
