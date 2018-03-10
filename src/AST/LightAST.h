#pragma once
#include "Expression.h"
#include "Value.h"

namespace kiwi{

/*
 * Define our light weight AST
 */
    class LightExpression {
    public:
        typedef typename std::string              StringType;
        typedef typename std::vector<Type<LightExpression>*> Args;
        typedef typename std::vector<StringType>  ArgNames;
        typedef          int                      IndexType;

        static std::string to_string(StringType str) {
            return str;
        }

        static StringType make_string(const std::string& str) {
            return StringType(str);
        }

        static StringType make_keyword(const std::string& str) {
            return StringType(str);
        }

        static StringType make_type_name(const std::string& str) {
            return StringType(str);
        }

        static StringType make_placeholder_name(const std::string& str) {
            return StringType(str);
        }

        static StringType make_function_name(const std::string& str) {
            return StringType(str);
        }

        static StringType make_error_message(const std::string& str) {
            return StringType(str);
        }

        static StringType make_builtin_name(const std::string& str) {
            return StringType(str);
        }

        static StringType make_argument_name(const std::string& str) {
            return StringType(str);
        }
    };

//*
template<typename NodeTrait>
struct AST{
    typedef Expression<NodeTrait> Node;
    typedef typename Call<NodeTrait> Call;

#define X(name, object) typedef object<NodeTrait> object;
    KIWI_AST_NODES
#undef X
};

typedef AST<LightExpression> LightAST;//*/

using ASTTrait = LightExpression;

}

