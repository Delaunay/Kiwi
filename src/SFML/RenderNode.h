#pragma once

#include <SFML/Graphics.hpp>

#include "../AST/Expression.h"
#include "../AST/LightAST.h"

#include "Fonts.h"

namespace kiwi{

class RenderNode{
public:
    typedef sf::Text                 StringType;
    typedef std::vector<RenderNode*> Args;
    typedef std::vector<StringType>  ArgNames;
    typedef int                      IndexType;

    template<typename T>
    static sf::Text make_text(const T& str, const sf::Color& color){
        sf::Text txt(str, default_font(), default_font_size());
        txt.setFillColor(color);
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
