#pragma once

#include "TypeExpression.h"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cassert>

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
    #define KIWI_AST_NODES\
        X(value, Value)\
        X(placeholder, Placeholder)\
        X(function, Function)\
        X(unary_call, UnaryCall)\
        X(binary_call, BinaryCall)\
        X(function_call, FunctionCall)\
        X(borrow, Borrow)\
        X(error, ErrorNode)

    enum class NodeTag {
    #define X(name, object) name,
        KIWI_AST_NODES
    #undef X
    };

    template<typename T>
    T acess(T* ptr) {
        if (ptr == nullptr) {

        }
        return ptr;
    }

    // Thanks gcc for forcing us to use such beautiful macro
    #define NODE_TYPES\
        typedef typename NodeTrait::StringType StringType;\
        typedef typename NodeTrait::Args       Args;\
        typedef typename NodeTrait::ArgNames   ArgNames;\
        typedef typename NodeTrait::IndexType  IndexType;

    // TODO: test typing system
    // TODO: Precedence table <= Or should I this is parsing stuff

    template<typename NodeTrait>
    class Expression: public NodeTrait {
    public:
        Expression(NodeTag tag) :
            tag(tag)
        {}

        const NodeTag tag;
        VTABLEV(virtual void visit(class DynamicVisitor* v) = 0;);
        PARENT(Expression<NodeTrait>* parent = nullptr;)
    };

    // Implementation of a function
    template<typename NodeTrait>
    class Function final : public Expression<NodeTrait> {
    public:
        NODE_TYPES;

        Function(const std::string& name, Expression<NodeTrait>* body) :
            Expression<NodeTrait>(NodeTag::function), name(NodeTrait::make_function_name(name)), body(body)
        {}

        Function(const std::string& name, Expression<NodeTrait>* body, Arrow<NodeTrait>* type) :
            Expression<NodeTrait>(NodeTag::function), name(NodeTrait::make_function_name(name)), body(body), type(type)
        {
            if (type) { assert(type->tag == NodeTypeTag::arrow && "Not a function type"); }
        }

        IndexType args_size() const {
            return int(args.size());
        }

        const StringType& arg(IndexType index) const {
            return args[index];
        }

        void add_arg(const std::string& str) {
            args.push_back(NodeTrait::make_argument_name(str));
        }

        VTABLEV(void visit(class DynamicVisitor* v) override;);

        StringType   name;
        ArgNames     args;
        Arrow<NodeTrait> *type{ nullptr };
        Expression<NodeTrait> *body{ nullptr };
    };

    /* Function calls are split in 3
    *      - call1, call to an unaryOperator
    *      - call2, call to a binaryOperator
    *      - calln, call to a function
    */
    template<typename NodeTrait>
    class Call : public Expression<NodeTrait> {
    public:
        NODE_TYPES;

        Arrow<NodeTrait>* type{ nullptr };
        StringType   name;

        std::size_t args_size() const;

        Expression<NodeTrait>* arg(IndexType index);

    protected:
        Call(NodeTag tag, const std::string& name) :
            Expression<NodeTrait>(tag), name(NodeTrait::make_string(name))
        {}
    };

    template<typename NodeTrait>
    class UnaryCall final : public Call<NodeTrait> {
    public:
        NODE_TYPES;

        UnaryCall(const std::string& name, Expression<NodeTrait>* expr) :
            Call<NodeTrait>(NodeTag::unary_call, name), expr(expr)
        {}

        IndexType args_size() const { return 1; }

        Expression<NodeTrait>* arg(IndexType index) {
            switch (index) {
            case 0:  return expr;
            default: return expr;
            }
        }

        VTABLEV(void visit(class DynamicVisitor* v) override;);
        Expression<NodeTrait>* expr;
        bool right = false; // operator is left/right associative
    };

    template<typename NodeTrait>
    class BinaryCall final : public Call<NodeTrait> {
    public:
        NODE_TYPES;

        BinaryCall(const std::string& name, Expression<NodeTrait>* lhs, Expression<NodeTrait>* rhs) :
            Call<NodeTrait>(NodeTag::binary_call, name), lhs(lhs), rhs(rhs)
        {}

        IndexType args_size() const { return 2; }

        Expression<NodeTrait>* arg(IndexType index) {
            switch (index) {
            case 0:  return lhs;
            case 1:  return rhs;
            default: return lhs;
            }
        }

        VTABLEV(void visit(class DynamicVisitor* v) override;);

        Expression<NodeTrait>* lhs;
        Expression<NodeTrait>* rhs;
    };

    template<typename NodeTrait>
    class FunctionCall final : public Call<NodeTrait> {
    public:
        NODE_TYPES;

        FunctionCall(const std::string& name, const std::vector<Expression<NodeTrait>*> &args) :
            Call<NodeTrait>(NodeTag::function_call, name), args(args)
        {}

        IndexType args_size() const {
            return int(args.size());
        }

        Expression<NodeTrait>* arg(IndexType index) {
            return args[index];
        }

        VTABLEV(void visit(class DynamicVisitor* v) override;);
        std::vector<Expression<NodeTrait>*> args;
    };

    template<typename NodeTrait>
    class Placeholder final : public Expression<NodeTrait> {
    public:
        NODE_TYPES;

        Placeholder(const std::string& name) :
            Expression<NodeTrait>(NodeTag::placeholder), name(NodeTrait::make_placeholder_name(name))
        {}

        VTABLEV(void visit(class DynamicVisitor* v) override;);
        
        StringType name;
        Expression<NodeTrait>* type{ nullptr };
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
    template<typename NodeTrait>
    class Borrow final : public Expression<NodeTrait> {
    public:
        NODE_TYPES;

        Borrow(Expression<NodeTrait>* expr) :
            Expression<NodeTrait>(NodeTag::borrow), expr(expr)
        {}

        VTABLEV(void visit(class DynamicVisitor* v) override;);
        Expression<NodeTrait>* expr{ nullptr };
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
    template<typename NodeTrait>
    class ErrorNode final : public Expression<NodeTrait> {
    public:
        NODE_TYPES;

        ErrorNode(const std::string& message, const std::string& code = "", Expression<NodeTrait>* partial = nullptr) :
            Expression<NodeTrait>(NodeTag::error), message(NodeTrait::make_error_message(message)), code(code), partial(partial)
        {}

        Expression<NodeTrait>* partial{ nullptr };    // partial parsed Node
        Expression<NodeTrait>* expected{ nullptr };   // suggested node
        StringType code;           // code the parser should use to solve the issue
        StringType message;        // error message
    };

    template<typename NodeTrait>
    Expression<NodeTrait>* Call<NodeTrait>::arg(IndexType index) {
        switch (Expression<NodeTrait>::tag) {
        case NodeTag::unary_call:
            return static_cast<UnaryCall<NodeTrait>&>(*this).arg(index);
        case NodeTag::binary_call:
            return static_cast<BinaryCall<NodeTrait>&>(*this).arg(index);
        case NodeTag::function_call:
            return static_cast<FunctionCall<NodeTrait>&>(*this).arg(index);
        default:
            return nullptr;
        }
    }

    template<typename NodeTrait>
    std::size_t Call<NodeTrait>::args_size() const {
        switch (Expression<NodeTrait>::tag) {
        case NodeTag::unary_call:
            return static_cast<const UnaryCall<NodeTrait>&>(*this).args_size();
        case NodeTag::binary_call:
            return static_cast<const BinaryCall<NodeTrait>&>(*this).args_size();
        case NodeTag::function_call:
            return static_cast<const FunctionCall<NodeTrait>&>(*this).args_size();
        default:
            return 0;
        }
    }
}