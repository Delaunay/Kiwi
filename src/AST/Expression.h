#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
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

    function,
    call1,
    call2,
    calln,

    // type
    type,   // type node
    arrow,  // function type

    // helpers
    builtin,
    borrow
};

class Arrow;

class Expression{
public:
    Expression(NodeTag tag):
        tag(tag)
    {}

    VTABLEV(virtual void visit(class DynamicVisitor* v) = 0;)

    NodeTag tag;
    PARENT(Expression* parent = nullptr;)
};

// Implementation of a function
class Function: public Expression{
public:
    typedef std::vector<std::string> ArgNames;

    Function(const std::string& name, Expression* body):
        Expression(NodeTag::function), name(name), body(body)
    {}

    std::size_t args_size() const{
        return args.size();
    }
    const std::string& arg(std::size_t index) const{
        return args[index];
    }

    VTABLEV(void visit(class DynamicVisitor* v) override;)
    std::string name;
    ArgNames    args;
    Arrow*      type;
    Expression* body;
};

/* Function Call are split in 3
 *      - call1, call to an unaryOperator
 *      - call2, call to a binaryOperator
 *      - calln, call to a function
 */
class Call: public Expression{
public:
    Arrow*      type;
    std::string name;

    std::size_t args_size() const;
    Expression* arg(std::size_t index);

protected:
    Call(NodeTag tag, const std::string& name):
        Expression(tag), name(name)
    {}
};

class UnaryCall: public Call{
public:
    UnaryCall(const std::string& name, Expression* expr):
        Call(NodeTag::call1, name), expr(expr)
    {}

    std::size_t args_size() const{ return 1;}

    Expression* arg(std::size_t index){
        switch(index){
        case 0:  return expr;
        default: return expr;
        }
    }

    VTABLEV(void visit(class DynamicVisitor* v) override;)
    Expression* expr;
    bool right = false; // operator is left/right associative
};

class BinaryCall: public Call{
public:
    BinaryCall(const std::string& name, Expression* lhs, Expression* rhs):
        Call(NodeTag::call2, name), lhs(lhs), rhs(rhs)
    {}

    std::size_t args_size() const{return 2;}

    Expression* arg(std::size_t index){
        switch(index){
        case 0:  return lhs;
        case 1:  return rhs;
        default: return lhs;
        }
    }

    VTABLEV(void visit(class DynamicVisitor* v) override;)
    Expression* lhs;
    Expression* rhs;
};

class FunctionCall: public Call{
public:
    typedef std::vector<Expression*> Args;

    FunctionCall(const std::string& name, const std::vector<Expression*>& args):
        Call(NodeTag::calln, name), args(args)
    {}

    std::size_t args_size() const{
        return args.size();
    }

    Expression* arg(std::size_t index){
        return args[index];
    }

    VTABLEV(void visit(class DynamicVisitor* v) override;)
    Args   args;
};

class Value: public Expression{
public:
    Value(double value):
        Expression(NodeTag::value), value(value)
    {}

    VTABLEV(void visit(class DynamicVisitor* v) override;)
    double value;

    Expression* evalue;
    Expression* type;
};

class Placeholder: public Expression{
public:
    Placeholder(const std::string name):
        Expression(NodeTag::placeholder), name(name)
    {}

    VTABLEV(void visit(class DynamicVisitor* v) override;)
    std::string name;
    Expression* type;
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
    const std::string name;
};


class Type: public Expression{
public:
    static Expression* type(const std::string& name){
        static std::unordered_map<std::string, Type> builtin_types = {
            {"type", Type("type")},
            // signed
            { "i8", Type( "i8")},
            {"i16", Type("i16")},
            {"i32", Type("i32")},
            {"i64", Type("i64")},
            // unsigned
            { "u8", Type( "u8")},
            {"u16", Type("u16")},
            {"u32", Type("u32")},
            {"u64", Type("u64")},
            // floats
            { "f8", Type( "f8")},
            {"f16", Type("f16")},
            {"f32", Type("f32")},
            {"f64", Type("f64")},
        };
        return &builtin_types[name];
    }

    Type(const std::string& name):
        Expression(NodeTag::type), name(name)
    {}

    Type():
        Expression(NodeTag::type)
    {}

    VTABLEV(void visit(class DynamicVisitor* v) override;)
    const std::string name;
};

// (i32, i32) -> i32
class Arrow: public Expression{
public:
    Arrow():
        Expression(NodeTag::arrow)
    {}

    VTABLEV(void visit(class DynamicVisitor* v) override;)
    std::vector<Expression*> args;
    Expression* return_type;
};


}
