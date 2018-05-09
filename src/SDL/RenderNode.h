#pragma once

#include "../AST/Expression.h"
#include "../AST/LightAST.h"
#include "SDLString.h"


namespace kiwi{

class RenderTrait{
public:
    typedef SDL_Color                SDLColor;
    typedef SDLString                StringType;
    typedef std::vector<Type<RenderTrait>*> ArgTypes;
    typedef std::vector<StringType>  ArgNames;
    typedef size_t                   IndexType;
    typedef SDLString                StringView;
    typedef std::string              StringArgument;

    static std::string to_string(StringType const& str) {
        return str.string();
    }

    template<typename T>
    static StringType make_text(const T& str, const SDLColor& color){
        StringType txt(str, color, StringType::default_font());
        return txt;
    }

    template<typename T>
    static StringType make_string(const T& str) {
        return make_text(str, { 255, 255, 255 });
    }

    template<typename T>
    static StringType make_call_name(const T& str) {
        return make_text(str, { 155, 155, 255 });
    }

    template<typename T>
    static StringType make_keyword(const T& str){
        return make_text(str, {255, 155, 155});
    }

    template<typename T>
    static StringType make_placeholder_name(const T& str){
        return make_text(str, {255, 155, 255});
    }

    template<typename T>
    static StringType make_function_name(const T& str){
        return make_text(str, {155, 255, 255});
    }

    template<typename T>
    static StringType make_error_message(const T& str){
        return make_text(str, {255, 155, 155});
    }

    template<typename T>
    static StringType make_builtin_name(const T& str){
        return make_text(str, {155, 255, 155});
    }

    template<typename T>
    static StringType make_argument_name(const T& str){
        return make_text(str, {155, 155, 155});
    }

	Point4f bound_box {10000, 10000, -10000, -10000};
};

//typedef AST<RenderNode> RenderTree;
//typedef AST<RenderTrait> RenderAST;

}
