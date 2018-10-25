#pragma once

#ifndef WIN32
#   include <algorithm>
#define MIN(x, y) std::min(x, y)
#define MAX(x, y) std::max(x, y)
#else
#define MIN(x, y) (((x) > (y)) ? (y) : (x))
#define MAX(x, y) (((x) < (y)) ? (y) : (x))
#endif

#include "../AST/Builder.h"
#include "../Logging/Log.h"

#include "RenderTreeVisitor.h"
#include "StyleManager.h"


namespace kiwi{

class RenderEngine: public RenderTreeVisitor<RenderEngine, sf::Vector2f, sf::Vector2f, int>{
public:
    typedef generic::Root<RenderAST::Node> Root;
    typedef Builder<Root, RenderAST> NBuilder;
    typedef typename RenderAST::Call Call;

    #define X(name, object) typedef typename RenderAST::object object;
        KIWI_AST_NODES
    #undef X

    RenderEngine(sf::RenderWindow& rw, sf::Vector2f pos = {0, 0}):
        rw(rw), original_pos(pos)
    { }

    sf::RenderWindow& rw;
    StyleManager style;
    float outline = 1;
    bool box_extracted = false;
    std::vector<std::pair<sf::FloatRect, Node*>> bounding_boxes;
    sf::FloatRect top_box = {10000, 10000, -10000, -10000};
    Node* current_expression = nullptr;
    sf::Vector2f original_pos = {0, 0};

    // Check if `render` inputs have changed
    // if so we need to recompute the bouding boxes and
    // update positions
    bool has_input_changed(Node* expr, sf::Vector2f pos){
        return (current_expression != expr || pos != original_pos);
    }

    void render(Node* expr, sf::Vector2f pos){
        if (has_input_changed(expr, pos)){
            current_expression = expr;
            original_pos = pos;
            bounding_boxes.clear();
            box_extracted = false;
        } else if (box_extracted){
            draw_bounding_box(top_box,
                              sf::Color(0, 0, 100),
                              sf::Color(000, 200, 100));
        }

        traverse(expr, pos, 0);

        // We need to take into account the current position
        if (!box_extracted){
            top_box.width  -= pos.x;
            top_box.height -= pos.y;
        }

        box_extracted = true;
    }

    void add_bounding_box(Node* parent, float x, float y, float w, float h){
        bounding_boxes.emplace_back(sf::FloatRect(x, y, w, h), parent);

        top_box.top    = MIN(x, top_box.top);
        top_box.left   = MIN(y, top_box.left);
        top_box.width  = MAX(x + w, top_box.width);
        top_box.height = MAX(y + h, top_box.height);
    }

    void draw_bounding_box(sf::FloatRect bb,
                          sf::Color fill = sf::Color(0, 100, 0),
                          sf::Color out = sf::Color(200, 100, 100)){
        sf::RectangleShape bbs({bb.width, bb.height});
        bbs.setPosition({bb.top, bb.left});
        bbs.setFillColor(fill);
        bbs.setOutlineThickness(outline);
        bbs.setOutlineColor(out);
        rw.draw(bbs);
    }

    void draw_bounding_box(float x, float y, float w, float h,
                          sf::Color fill = sf::Color(0, 100, 0),
                          sf::Color out = sf::Color(200, 100, 100)){
        return draw_bounding_box({y, x, w, h}, fill, out);
    }


    // helpers
    // ------------------------------------------------------------------------
    sf::Vector2f render(Node* parent, sf::Text item, sf::Vector2f pos){
        //log_trace("Rendering: " + item.getString());
        item.setPosition(pos.x, pos.y);

        float x = pos.x;
        float y = pos.y;
        float w = style.width(item);
        float h = style.height() - outline;

        draw_bounding_box(x, y, w, h);

        if (!box_extracted)
            add_bounding_box(parent, x, y, w, h);

        rw.draw(item);
        pos.x += w;
        return pos;
    }

    sf::Vector2f new_line(sf::Vector2f pos, int indent){
        pos.y += style.height();
        pos.x = original_pos.x + style.width() * indent;
        return pos;
    }

    static Root make_sqr(){
        Root x    = NBuilder::placeholder("x");
        Root body = NBuilder::mult(x, NBuilder::borrow(x));

        Root dbl = NBuilder::builtin("double");
        Root fun_type = NBuilder::arrow({dbl.get()}, dbl.get());

        Root sqr  = NBuilder::function("sqr", body, fun_type);
        RenderNode* node = sqr.get();

        // generic::Function<RenderNode>*
        Function* f = static_cast<Function*>(node);
        f->add_arg("x");

        return sqr;
    }

    // ------------------------------------------------------------------------
    static sf::Vector2f run(sf::RenderWindow& rw, RenderNode* expr, sf::Vector2f pos){
        log_trace("Initializing");
        RenderEngine eval(rw, pos);
        log_trace("Tree Renderer starting");
        return eval.traverse(expr, pos, 0);
    }

    sf::Vector2f function(Function* x, sf::Vector2f pos, int idt){
        pos = render(x, style.def(), pos);
        pos = render(x, x->name, pos);
        pos = render(x, style.get('('), pos);

        int na = 0;

        if (x->type != nullptr)
            na = x->type->args_size() - 1;

        int n = int(x->args_size()) - 1;

        assert(n == na && "Arrow type size mistmatch");

        if (n >= 0){

            for(int i = 0; i < n; ++i){
                pos = render(x, x->arg(i), pos);
                pos = render(x, style.get(':'), pos);
                pos = traverse(x->type->arg(i), pos, idt);
                pos = render(x, style.get(','), pos);
            }
            pos = render(x, x->arg(n), pos);
            pos = render(x, style.get(':'), pos);
            pos = traverse(x->type->arg(na), pos, idt);
        }

        pos = render(x, style.get(')'), pos);

        if (x->type != nullptr && x->type->return_type != nullptr){
            pos = render(x, style.arrow(), pos);
            pos = traverse(x->type->return_type, pos, idt);
        }

        pos = render(x, style.get(':'), pos);
        pos = new_line(pos);
        pos = indent(pos, idt + 1);
        return traverse(x->body, pos, idt + 1);
    }

    sf::Vector2f new_line(sf::Vector2f pos){
        return {original_pos.x, pos.y + style.height()};
    }

    sf::Vector2f indent(sf::Vector2f pos, int idt){
        return sf::Vector2f(original_pos.x + style.width() * 4 * idt , pos.y);
    }

    sf::Vector2f function_call(FunctionCall* x, sf::Vector2f pos, int idt){
        pos = render(x, x->name, pos);
        pos = render(x, style.get('('), pos);

        int n = x->args_size() - 1;

        for(int i = 0; i < n; ++i){
            pos = traverse(x->arg(i), pos, idt);
            pos = render(x, style.get(','), pos);
        }

        pos = traverse(x->arg(n), pos, idt);
        pos = render(x, style.get(')'), pos);
        return pos;
    }

    sf::Vector2f unary_call(UnaryCall* x, sf::Vector2f pos, int idt){
        if (!x->right)
            pos = render(x, x->name, pos);

        pos = traverse(x->arg(0), pos, idt);

        if (x->right)
            pos = render(x, x->name, pos);

        return pos;
    }

    sf::Vector2f binary_call(BinaryCall* x, sf::Vector2f pos, int idt){
        pos = traverse(x->lhs, pos, idt);
        pos = render(x, style.get(' '), pos);
        pos = render(x, x->name, pos);
        pos = render(x, style.get(' '), pos);
        pos = traverse(x->rhs, pos, idt);
        return pos;
    }

    sf::Vector2f borrow(Borrow* b, sf::Vector2f pos, int idt){
        return traverse(b->expr, pos, idt);
    }

    sf::Vector2f arrow(Arrow* x, sf::Vector2f pos, int idt){
        pos = render(x, style.get('('), pos);

        int n = int(x->args.size()) - 1;
        for(int i = 0; i < n; ++i){
            pos = traverse(x->arg(i), pos, idt);
            pos = render(x, style.get(','), pos);
        }
        pos = traverse(x->arg(n), pos, idt);
        pos = render(x, style.get(')'), pos);
        pos = render(x, style.arrow(), pos);
        pos = traverse(x->return_type, pos, idt);
        return pos;
    }

    sf::Vector2f type(Type* x, sf::Vector2f pos, int){
        return render(x, x->name, pos);
    }

    sf::Vector2f builtin(Builtin* e, sf::Vector2f pos, int){
        return render(e, e->name, pos);
    }

    sf::Vector2f error(ErrorNode* e, sf::Vector2f pos, int){
        return render(e, e->message, pos);
    }

    sf::Vector2f value(Value* x, sf::Vector2f pos, int){
        sf::Text txt(std::to_string(x->as<f64>()), style.font(), 10);
        return render(x, txt, pos);
    }

    sf::Vector2f placeholder(Placeholder* x, sf::Vector2f pos, int){
        return render(x, x->name, pos);
    }
};

}
