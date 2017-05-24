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
 *
 * Everything is a template because we are going to generate different ASTs
 * i.e RenderTree to render the AST on the screen and a Lighweight AST
 * for internal use
 */
namespace kiwi{


#define KIWI_AST_NODES\
    X(value, Value)\
    X(placeholder, Placeholder)\
    X(function, Function)\
    X(unary_call, UnaryCall)\
    X(binary_call, BinaryCall)\
    X(function_call, FunctionCall)\
    X(type, Type)\
    X(arrow, Arrow)\
    X(builtin, Builtin)\
    X(borrow, Borrow)\
    X(error, ErrorNode)

enum class NodeTag{
#define X(name, object) name,
    KIWI_AST_NODES
#undef X
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

namespace generic{

template<typename Node>
class NodeTrait{
public:
    typedef std::string             StringType;
    typedef std::vector<Node*>      Args;
    typedef std::vector<StringType> ArgNames;
    typedef std::size_t             IndexType;
};

template<typename Node>
class BaseNode{
public:
    typedef typename NodeTrait<Node>::StringType StringType;
    typedef typename NodeTrait<Node>::Args       Args;
    typedef typename NodeTrait<Node>::ArgNames   ArgNames;
    typedef typename NodeTrait<Node>::IndexType  IndexType;
};

// (i32, i32) -> i32
template<typename Node>
class Arrow final: public Node, public BaseNode<Node>{
public:
    Arrow():
        Node(NodeTag::arrow)
    {}

    VTABLEV(void visit(class DynamicVisitor* v) override;)
    std::vector<Node*> args;
    Node* return_type;
};

// Implementation of a function
template<typename Node>
class Function final: public Node, public BaseNode<Node>{
public:
    Function(const StringType& name, Node* body):
        Node(NodeTag::function), name(name), body(body)
    {}

    IndexType args_size() const{
        return args.size();
    }
    const StringType& arg(IndexType index) const{
        return args[index];
    }

    VTABLEV(void visit(class DynamicVisitor* v) override;)

    StringType   name;
    ArgNames     args;
    Arrow<Node>* type;
    Node       * body;
};

/* Function Call are split in 3
 *      - call1, call to an unaryOperator
 *      - call2, call to a binaryOperator
 *      - calln, call to a function
 */
template<typename Node>
class Call: public Node, public BaseNode<Node>{
public:
    Arrow<Node>* type;
    StringType   name;

    IndexType args_size() const{
        switch(tag){
        case NodeTag::unary_call:
            return static_cast<const UnaryCall<Node>&>(*this).args_size();
        case NodeTag::binary_call:
            return static_cast<const BinaryCall<Node>&>(*this).args_size();
        case NodeTag::function_call:
            return static_cast<const FunctionCall<Node>&>(*this).args_size();
        default:
            return 0;
        }
    }

    Node* arg(IndexType index);

protected:
    Call(NodeTag tag, const StringType& name):
        Node(tag), name(name)
    {}
};

template<typename Node>
class UnaryCall final: public Call<Node>{
public:
    UnaryCall(const StringType& name, Node* expr):
        Call(NodeTag::unary_call, name), expr(expr)
    {}

    IndexType args_size() const{ return 1;}

    Node* arg(IndexType index){
        switch(index){
        case 0:  return expr;
        default: return expr;
        }
    }

    VTABLEV(void visit(class DynamicVisitor* v) override;)
    Node* expr;
    bool right = false; // operator is left/right associative
};

template<typename Node>
class BinaryCall final: public Call<Node>{
public:
    BinaryCall(const StringType& name, Node* lhs, Node* rhs):
        Call(NodeTag::binary_call, name), lhs(lhs), rhs(rhs)
    {}

    IndexType args_size() const{return 2;}

    Node* arg(IndexType index){
        switch(index){
        case 0:  return lhs;
        case 1:  return rhs;
        default: return lhs;
        }
    }

    VTABLEV(void visit(class DynamicVisitor* v) override;)
    Node* lhs;
    Node* rhs;
};

template<typename Node>
class FunctionCall final: public Call<Node>{
public:
    FunctionCall(const StringType& name, const Args& args):
        Call(NodeTag::function_call, name), args(args)
    {}

    IndexType args_size() const{
        return args.size();
    }

    Node* arg(IndexType index){
        return args[index];
    }

    VTABLEV(void visit(class DynamicVisitor* v) override;)
    Args   args;
};


template<typename Node>
class Placeholder final: public Node, public BaseNode<Node>{
public:
    Placeholder(const StringType name):
        Node(NodeTag::placeholder), name(name)
    {}

    VTABLEV(void visit(class DynamicVisitor* v) override;)
    StringType name;
    Node* type;
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
template<typename Node>
class Borrow final: public Node, public BaseNode<Node>{
public:
    Borrow(Node* expr):
        Node(NodeTag::borrow), expr(expr)
    {}

    VTABLEV(void visit(class DynamicVisitor* v) override;)
    Node* expr;
};

/* Builtin is a special construct used to implement
 * builtin operations i.e (+/x/-/import etc...)
 * that are hardcoded inside the compiler itself.
 *
 * During eval the compiler will lookup the implementation
 */
template<typename Node>
class Builtin final: public Node, public BaseNode<Node>{
public:
    Builtin(const StringType& name):
        Node(NodeTag::builtin), name(name)
    {}

    VTABLEV(void visit(class DynamicVisitor* v) override;)
    const StringType name;
};


template<typename Node>
class Type final: public Node, public BaseNode<Node>{
public:
    Type(const StringType& name):
        Node(NodeTag::type), name(name)
    {}

    Type():
        Node(NodeTag::type)
    {}

    VTABLEV(void visit(class DynamicVisitor* v) override;)
    const StringType name;
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
template<typename Node>
class ErrorNode final: public Node, public BaseNode<Node>{
public:
    ErrorNode(const StringType message, Node* partial=nullptr):
        Node(NodeTag::error), message(message), partial(partial)
    {}

    Node* partial;    // partial parsed Node
    Node* expected;   // suggested node
    StringType code;       // code the parser should use to solve the issue
    StringType message;    // error message
};


template<typename Node>
Node* Call<Node>::arg(IndexType index){
    switch(tag){
    case NodeTag::unary_call:
        return static_cast<UnaryCall<Node>&>(*this).arg(index);
    case NodeTag::binary_call:
        return static_cast<BinaryCall<Node>&>(*this).arg(index);
    case NodeTag::function_call:
        return static_cast<FunctionCall<Node>&>(*this).arg(index);
    default:
        return nullptr;
    }
}

}
}
