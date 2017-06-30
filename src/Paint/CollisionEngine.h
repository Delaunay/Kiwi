#pragma once

#include "RenderTreeVisitor.h"
#include "StyleManager.h"

#include <SFML/Graphics.hpp>

namespace kiwi{

class CollisionExtractor: public RenderTreeVisitor<CollisionExtractor, void>{
public:
    typedef void RetType;

    //* To be implemented:
    RetType function(Function* x);
    RetType function_call(FunctionCall* x);
    RetType unary_call(UnaryCall* x);
    RetType binary_call(BinaryCall* x);
    RetType borrow(Borrow* b);
    RetType arrow(Arrow* x);
    RetType type(Type* x);
    RetType builtin(Builtin* e);
    RetType error(ErrorNode* e);
    RetType value(Value* x);

    RetType placeholder(Placeholder* x){
        style.get(x->name);
    }

    std::vector<sf::FloatRect>& bbs;
    StyleManager& style;
};

class CollisionEngine{
public:
    CollisionEngine() = default;



    std::vector<sf::FloatRect> bouding_boxes;
};

}
