#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cassert>
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

template<typename T>
T acess(T* ptr){
    if (ptr == nullptr){

    }
    return ptr;
}

// TODO: test typing system
// TODO: Precedence table <= Or should I this is parsing stuff

class Expression{
public:
    typedef typename std::string              StringType;
    typedef typename std::vector<Expression*> Args;
    typedef typename std::vector<StringType>  ArgNames;
    typedef          int                      IndexType;

    static StringType make_string(const std::string& str){
        return StringType(str);
    }

    static StringType make_keyword(const std::string& str){
        return StringType(str);
    }

    static StringType make_placeholder_name(const std::string& str){
        return StringType(str);
    }

    static StringType make_function_name(const std::string& str){
        return StringType(str);
    }

    static StringType make_error_message(const std::string& str){
        return StringType(str);
    }

    static StringType make_builtin_name(const std::string& str){
        return StringType(str);
    }

    static StringType make_argument_name(const std::string& str){
        return StringType(str);
    }

    Expression(NodeTag tag):
        tag(tag)
    {}

    VTABLEV(virtual void visit(class DynamicVisitor* v) = 0;)

    NodeTag tag;
    PARENT(Expression* parent = nullptr;)
};

namespace generic{

// Thanks gcc for forcing us to use such beautiful macro
#define NODE_TYPES\
    typedef typename Node::StringType StringType;\
    typedef typename Node::Args       Args;\
    typedef typename Node::ArgNames   ArgNames;\
    typedef typename Node::IndexType  IndexType;

// (i32, i32) -> i32
template<typename Node>
class Arrow final: public Node{
public:
    NODE_TYPES

    Arrow():
        Node(NodeTag::arrow)
    {}

    Arrow(const Args& args, Node* expr):
        Node(NodeTag::arrow), args(args), return_type(expr)
    {}

    int args_size()  { return args.size(); }
    Node* arg(int i) { return args[i]; }

    VTABLEV(void visit(class DynamicVisitor* v) override;)
    Args  args;
    Node* return_type{nullptr};
};

// Implementation of a function
template<typename Node>
class Function final: public Node{
public:
    NODE_TYPES

    Function(const std::string& name, Node* body):
        Node(NodeTag::function), name(Node::make_function_name(name)), body(body)
    {}

    Function(const std::string& name, Node* body, Node* type):
        Node(NodeTag::function), name(Node::make_function_name(name)), body(body), type(static_cast<Arrow<Node>*>(type))
    {
        assert(type->tag == NodeTag::arrow && "Not a function type");
    }

    IndexType args_size() const{
        return int(args.size());
    }

    const StringType& arg(IndexType index) const{
        return args[index];
    }

    void add_arg(const std::string& str){
        args.push_back(Node::make_argument_name(str));
    }

    VTABLEV(void visit(class DynamicVisitor* v) override;)

    StringType   name;
    ArgNames     args;
    Arrow<Node>* type{nullptr};
    Node        *body{nullptr};
};

/* Function Call are split in 3
 *      - call1, call to an unaryOperator
 *      - call2, call to a binaryOperator
 *      - calln, call to a function
 */
template<typename Node>
class Call: public Node{
public:
    NODE_TYPES

    Arrow<Node>* type{nullptr};
    StringType   name;

    std::size_t args_size() const;

    Node* arg(IndexType index);

protected:
    Call(NodeTag tag, const std::string& name):
        Node(tag), name(Node::make_string(name))
    {}
};

template<typename Node>
class UnaryCall final: public Call<Node>{
public:
    NODE_TYPES

    UnaryCall(const std::string& name, Node* expr):
        Call<Node>(NodeTag::unary_call, Node::make_string(name)), expr(expr)
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
    NODE_TYPES

    BinaryCall(const std::string& name, Node* lhs, Node* rhs):
        Call<Node>(NodeTag::binary_call, name), lhs(lhs), rhs(rhs)
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
    NODE_TYPES

    FunctionCall(const std::string& name, const Args& args):
        Call<Node>(NodeTag::function_call, Node::make_string(name)), args(args)
    {}

    IndexType args_size() const{
        return int(args.size());
    }

    Node* arg(IndexType index){
        return args[index];
    }

    VTABLEV(void visit(class DynamicVisitor* v) override;)
    Args   args;
};


template<typename Node>
class Placeholder final: public Node{
public:
    NODE_TYPES

    Placeholder(const std::string& name):
        Node(NodeTag::placeholder), name(Node::make_placeholder_name(name))
    {}

    VTABLEV(void visit(class DynamicVisitor* v) override;)
    StringType name;
    Node* type{nullptr};
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
class Borrow final: public Node{
public:
    NODE_TYPES

    Borrow(Node* expr):
        Node(NodeTag::borrow), expr(expr)
    {}

    VTABLEV(void visit(class DynamicVisitor* v) override;)
    Node* expr{nullptr};
};

/* Builtin is a special construct used to implement
 * builtin operations i.e (+/x/-/import etc...)
 * that are hardcoded inside the compiler itself.
 *
 * During eval the compiler will lookup the implementation
 */
template<typename Node>
class Builtin final: public Node{
public:
    NODE_TYPES

    Builtin(const std::string& name):
        Node(NodeTag::builtin), name(Node::make_builtin_name(name))
    {}

    VTABLEV(void visit(class DynamicVisitor* v) override;)
    const StringType name;
};


template<typename Node>
class Type final: public Node{
public:
    NODE_TYPES

    Type(const std::string& name):
        Node(NodeTag::type), name(Node::make_type_name(name))
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
class ErrorNode final: public Node{
public:
    NODE_TYPES

    ErrorNode(const std::string& message, Node* partial=nullptr):
        Node(NodeTag::error), message(Node::make_error_message(message)), partial(partial)
    {}

    Node* partial{nullptr};    // partial parsed Node
    Node* expected{nullptr};   // suggested node
    StringType code;           // code the parser should use to solve the issue
    StringType message;        // error message
};


template<typename Node>
Node* Call<Node>::arg(IndexType index){
    switch(Node::tag){
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

template<typename Node>
std::size_t Call<Node>::args_size() const{
    switch(Node::tag){
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

}
}
