#pragma once

#include "../AST/Expression.h"
#include "../AST/LightAST.h"
#include "SDLString.h"


namespace kiwi{

class RenderNode{
public:
    typedef SDL_Color                SDLColor;
    typedef SDLString                StringType;
    typedef std::vector<RenderNode*> Args;
    typedef std::vector<StringType>  ArgNames;
    typedef int                      IndexType;

    template<typename T>
    static StringType make_text(const T& str, const SDLColor& color){
        StringType txt(str, color, StringType::default_font());
        return txt;
    }

    RenderNode(NodeTag tag):
        tag(tag)
    {}

    template<typename T>
    static StringType make_string(const T& str){
        return make_text(str, {255, 255, 255});
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

    NodeTag tag;
    PARENT(RenderNode* parent = nullptr;)
};

//typedef AST<RenderNode> RenderTree;
typedef AST<RenderNode> RenderAST;

}
