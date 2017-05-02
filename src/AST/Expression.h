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

    function,
    call,

    builtin,
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

// Implementation of a function
class Function: public Expression{
public:
    std::string name;
    std::vector<std::string> args;
    Expression* body;

    Function(const std::string& name, Expression* body):
        Expression(NodeTag::function), name(name), body(body)
    {}

    VTABLEV(void visit(class DynamicVisitor* v) override;)


    std::size_t args_size() const{
        return args.size();
    }
    const std::string& arg(std::size_t index) const{
        return args[index];
    }
};

// Reference to a function
class FunctionCall: public Expression{
public:
    std::string name;
    std::vector<Expression*> args;

    /*
    FunctionCall(std::size_t arg_size):
        Expression(NodeTag::call), args(arg_size)
    {}*/

    FunctionCall(const std::string& name, const std::vector<Expression*>& args):
        Expression(NodeTag::call), name(name), args(args)
    {}

    std::size_t args_size() const{
        return args.size();
    }

    Expression* arg(std::size_t index){
        return args[index];
    }

    VTABLEV(void visit(class DynamicVisitor* v) override;)
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

    VTABLEV(void visit(class DynamicVisitor* v) override;)

    Expression* expr;
};


/* Builtin is a special construct used to implement
 * builtin operations i.e (+/x/-/import etc...)
 * that are hardcoded inside the compiler itself.
 *
 * During eval the compiler will lookup the implementation
 */
class Builtin: public Expression{
public:
    Builtin(const std::string& name):
        Expression(NodeTag::builtin), name(name)
    {}

    VTABLEV(void visit(class DynamicVisitor* v) override;)
    std::string name;
};


}
