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

    RenderNode(NodeTag tag):
        tag(tag)
    {}

    static StringType make_string(const std::string& str){
        return StringType(str, default_font(), default_font_size());
    }

    NodeTag tag;
    PARENT(RenderNode* parent = nullptr;)
};

//typedef AST<RenderNode> RenderTree;
typedef AST<RenderNode> RenderAST;

}
