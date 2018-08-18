#ifndef KIWI_AST_DECLARATIONS_NODE_HEADER
#define KIWI_AST_DECLARATIONS_NODE_HEADER

#include <cassert>

#include "Expression.h"
#include "Statement.h"
#include "Type.h"

namespace kiwi {

class Definition : public Statement {
  public:
    Definition(NodeTag tag, String name, Expression *type = nullptr) :
        Statement(tag), name(name), type(type) {}

    const String name;
    Expression *type;
};

// def fun(args)-> ret:
//      body
class FunctionDefinition : public Definition {
  public:
    FunctionDefinition(String const &name, Expression *body) :
        Definition(NodeTag::function_def, name), body(body) {}

    FunctionDefinition(String const &name, Expression *body, Expression *ftype) :
        Definition(NodeTag::function_def, name, ftype), body(body) {}

    u64 args_size() const { return args.size(); }

    const String &arg(u64 index) const { return args[index]; }

    void add_arg(String const &str) { args.push_back(str); }

    Array<String> args;
    Expression *body{nullptr};
};

// macro fun(args):
//      body
//
//  a macro is basically a function that is executed at compile time
//  that returns an expression to be compiled
//
//  We do not need a separate definition for macros since macros are functions
//  but having it as a special case might help later on ?
//
class MacroDefinition : public Definition {
  public:
    MacroDefinition(String const &name, Expression *body) :
        Definition(NodeTag::macro_def, name), body(body) {}

    MacroDefinition(String const &name, Expression *body, Expression *ftype) :
        Definition(NodeTag::macro_def, name, ftype), body(body) {}

    u64 args_size() const { return args.size(); }

    const String &arg(u64 index) const { return args[index]; }

    void add_arg(String const &str) { args.push_back(str); }

    Array<String> args;
    Expression *body{nullptr};
};

// User defined Type
// ----------------------------------
// Tuple like syntax:
//
//      MyType = (my_x: Int, my_y: Int)
//
// ----------------------------------
//  OOP like syntax:
//
//      class MyType:
//          my_x: Int
//          my_y: Int
//
//  Functional
//
//      class MyType(my_x: Int, my_u: Int)
//
// We allow the users to defined methods.
// Methods are functions that always have at leat one argument
// The first agurment is always a reference of type `object`
// Since methods are namespaced functions we do not support
// virtual calls and as such we do not support OOP (:D)
//

// (name: Type * name: Type)

// struct type_name(name: meta_type):
//      name: type
class StructDefinition : public Definition {
  public:
    StructDefinition(String const &name) : Definition(NodeTag::struct_def, name) {}

    Array<Tuple<String, Statement *>> attributes;
    Array<Tuple<String, Statement *>> meta_types;
};

// union type_name(name: meta_type):
//      name: type
class UnionDefinition : public Definition {
  public:
    UnionDefinition(String const &name) : Definition(NodeTag::union_def, name) {}

    Array<Tuple<String, Statement *>> attributes;
    Array<Tuple<String, Statement *>> meta_types;
};

} // namespace kiwi

#endif
