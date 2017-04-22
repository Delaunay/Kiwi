#pragma once

#include <iostream>
#include <string>
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
    add,
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


class Add: public Expression{
public:
    Add(Expression* lhs, Expression* rhs):
        Expression(NodeTag::add), lhs(lhs), rhs(rhs)
    {}

    VTABLEV(void visit(class DynamicVisitor* v) override;)

    Expression* lhs;
    Expression* rhs;
};

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
 */
class Borrow: public Expression{
public:
    Borrow(Expression* expr):
        Expression(NodeTag::borrow), expr(expr)
    {}

    Expression* expr;
};


}
