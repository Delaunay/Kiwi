#pragma once

#include "../AST/Builder.h"

#include "RenderTreeVisitor.h"
#include "Fonts.h"

namespace kiwi{

class RenderEngine: public RenderTreeVisitor<RenderEngine, sf::Vector2f, sf::Vector2f, int>{
public:
    typedef generic::Root<RenderAST::Node> Root;
    typedef Builder<Root, RenderAST> NBuilder;
    typedef typename RenderAST::Call Call;

    #define X(name, object) typedef typename RenderAST::object object;
        KIWI_AST_NODES
    #undef X

    RenderEngine(sf::RenderWindow& rw, sf::Vector2f pos):
        rw(rw), original_pos(pos)
    {}

    sf::Font font = default_font();

    unsigned int font_size = default_font_size();
    sf::Text fun_def = sf::Text("def ", font, font_size);
    sf::Text fun_op  = sf::Text("(", font, font_size);
    sf::Text fun_cp  = sf::Text(")", font, font_size);
    sf::Text fun_col = sf::Text(":", font, font_size);
    sf::Text fun_com = sf::Text(", ", font, font_size);
    sf::Text space   = sf::Text(" ", font, font_size);
    sf::Text farrow  = sf::Text(L" \u2192 ", font, font_size);

    float glyph_width = space.getGlobalBounds().width;   // space.getCharacterSize();
    float glyph_height = font.getLineSpacing(font_size); // space.getGlobalBounds().height;

    std::ostream& out = std::cout;
    sf::RenderWindow& rw;
    sf::Vector2f original_pos;

    // helpers
    // ------------------------------------------------------------------------
    sf::Vector2f render(sf::Text item, sf::Vector2f pos){
        item.setPosition(pos.x, pos.y);
        rw.draw(item);
        pos.x += glyph_width * item.getString().getSize();
        return pos;
    }

    sf::Vector2f new_line(sf::Vector2f pos, int indent){
        pos.y += glyph_height;
        pos.x = original_pos.x + glyph_width * indent;
        return pos;
    }

    static Root make_sqr(){
        Root x    = NBuilder::placeholder("x");
        Root body = NBuilder::mult(x, NBuilder::borrow(x));
        Root sqr  = NBuilder::function("sqr", body);
        RenderNode* node = sqr.get();

        // generic::Function<RenderNode>*
        Function* f = static_cast<Function*>(node);
        f->args.push_back(sf::Text("x", default_font(), default_font_size()));

        return sqr;
    }

    // ------------------------------------------------------------------------
    static sf::Vector2f run(sf::RenderWindow& rw, RenderNode* expr, sf::Vector2f pos){
        RenderEngine eval(rw, pos);
        return eval.traverse(expr, pos, 0);
    }

    sf::Vector2f function(Function* x, sf::Vector2f pos, int idt){
        pos = render(fun_def, pos);
        pos = render(x->name, pos);
        pos = render(fun_op, pos);


        int n = int(x->args_size()) - 1;
        if (n >= 0){
            for(int i = 0; i < n; ++i){
                pos = render(x->arg(i), pos);
                pos = render(fun_com, pos);
            }
            pos = render(x->arg(n), pos);
        }

        pos = render(fun_cp, pos);


        if (x->type != nullptr && x->type->return_type != nullptr){
            pos = render(farrow, pos);
            pos = traverse(x->type->return_type, pos, idt);
        }

        pos = render(fun_col, pos);
        pos = new_line(pos);
        pos = indent(pos, idt + 1);
        return traverse(x->body, pos, idt + 1);
    }

    sf::Vector2f new_line(sf::Vector2f pos){
        return pos + sf::Vector2f(0, glyph_height);
    }

    sf::Vector2f indent(sf::Vector2f pos, int idt){
        return sf::Vector2f(glyph_width * 4 * idt , pos.y);
    }

    sf::Vector2f function_call(FunctionCall* x, sf::Vector2f pos, int idt){
        pos = render(x->name, pos);
        pos = render(fun_op, pos);

        int n = x->args_size() - 1;
        for(int i = 0; i < n; ++i){
            pos = traverse(x->arg(i), pos, idt);
            pos = render(fun_com, pos);
        }

        pos = traverse(x->arg(n), pos, idt);
        pos = render(fun_cp, pos);
        return pos;
    }

    sf::Vector2f unary_call(UnaryCall* x, sf::Vector2f pos, int idt){
        if (!x->right)
            pos = render(x->name, pos);

        pos = traverse(x->arg(0), pos, idt);

        if (x->right)
            pos = render(x->name, pos);

        return pos;
    }

    sf::Vector2f binary_call(BinaryCall* x, sf::Vector2f pos, int idt){
        pos = traverse(x->lhs, pos, idt);
        pos = render(x->name, pos);
        pos = traverse(x->rhs, pos, idt);
        return pos;
    }

    sf::Vector2f borrow(Borrow* b, sf::Vector2f pos, int idt){
        return traverse(b->expr, pos, idt);
    }

    sf::Vector2f arrow(Arrow* x, sf::Vector2f pos, int idt){
        pos = render(fun_op, pos);

        int n = int(x->args.size()) - 1;
        for(int i = 0; i < n; ++i){
            pos = traverse(x->arg(i), pos, idt);
            out << ", ";
        }
        pos = traverse(x->arg(n), pos, idt);
        pos = render(fun_cp, pos);
        pos = render(farrow, pos);
        pos = traverse(x->return_type, pos, idt);
        return pos;
    }

    sf::Vector2f type(Type* x, sf::Vector2f pos, int){
        return render(x->name, pos);
    }

    sf::Vector2f builtin(Builtin* e, sf::Vector2f pos, int){
        return render(e->name, pos);
    }

    sf::Vector2f error(ErrorNode* e, sf::Vector2f pos, int){
        return render(e->message, pos);
    }

    sf::Vector2f value(Value* x, sf::Vector2f pos, int){
        sf::Text txt(std::to_string(x->as<f64>()), font, 10);
        return render(txt, pos);
    }

    sf::Vector2f placeholder(Placeholder* x, sf::Vector2f pos, int){
        return render(x->name, pos);
    }
};

}
