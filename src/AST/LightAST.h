#pragma once
#include "Expression.h"
#include "Value.h"

namespace kiwi {

/*
 * Define our light weight AST
 */
class LightExpression {
  public:
    typedef typename std::string StringType;
    typedef typename std::vector<Type *> ArgTypes;
    typedef typename std::vector<StringType> ArgNames;
    typedef size_t IndexType;
    typedef std::string StringView;
    typedef std::string StringArgument;

    static std::string to_string(StringType str) { return str; }

    static StringType make_call_name(StringArgument str) { return StringType(str); }

    static StringType make_string(StringArgument str) { return StringType(str); }

    static StringType make_keyword(StringArgument str) { return StringType(str); }

    static StringType make_type_name(StringArgument str) { return StringType(str); }

    static StringType make_placeholder_name(StringArgument str) { return StringType(str); }

    static StringType make_function_name(StringArgument str) { return StringType(str); }

    static StringType make_error_message(StringArgument str) { return StringType(str); }

    static StringType make_builtin_name(StringArgument str) { return StringType(str); }

    static StringType make_argument_name(StringArgument str) { return StringType(str); }
};

/*
template <typename NodeTrait> struct AST {
    typedef Expression Node;
    typedef typename Call Call;

#define X(name, object) typedef object object;
    KIWI_AST_NODES(X)
#undef X
};* /

typedef AST<LightExpression> LightAST; // */

using ASTTrait = LightExpression;

} // namespace kiwi
