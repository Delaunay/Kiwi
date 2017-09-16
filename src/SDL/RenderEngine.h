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


class RenderEngine: public RenderTreeVisitor<RenderEngine, Point, Point, int>{
public:
    typedef SDL_Renderer* RenderSurface;
    typedef SDLString StringType;
    typedef SDL_Font* FontType;
    typedef generic::Root<RenderAST::Node> Root;
    typedef Builder<Root, RenderAST> NBuilder;
    typedef typename RenderAST::Call Call;

    #define X(name, object) typedef typename RenderAST::object object;
        KIWI_AST_NODES
    #undef X

    RenderEngine(RenderSurface& rw, Point pos = {0, 0}):
        rw(rw), original_pos(pos)
    { }

    RenderSurface& rw;
    float outline = 1;
    bool box_extracted = false;
    std::vector<std::pair<Rectangle, Node*>> bounding_boxes;
    const StyleManager& style = StyleManager::style_manager();
    Rectangle top_box = {10000, 10000, -10000, -10000};
    Node* current_expression = nullptr;
    Point original_pos = {0, 0};

    // Check if `render` inputs have changed
    // if so we need to recompute the bouding boxes and
    // update positions
    bool has_input_changed(Node* expr, Point pos){
        return (current_expression != expr || pos != original_pos);
    }

    void render(Node* expr, Point pos){
        if (has_input_changed(expr, pos)){
            current_expression = expr;
            original_pos = pos;
            bounding_boxes.clear();
            box_extracted = false;
        } else if (box_extracted){
            draw_bounding_box(top_box,
                              Color{0, 0, 100},
                              Color{000, 200, 100});
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
        bounding_boxes.emplace_back(Rectangle(x, y, w, h), parent);

        top_box.x      = MIN(x, top_box.x);
        top_box.y      = MIN(y, top_box.y);
        top_box.width  = MAX(x + w, top_box.width);
        top_box.height = MAX(y + h, top_box.height);
    }

    void draw_bounding_box(Rectangle bb,
                           Color fill = Color(0, 100, 0),
                           Color out = Color(200, 100, 100)){
        SDL_Rect shape = {bb.x, bb.y, bb.width, bb.height};
        SDL_SetRenderDrawColor(rw, out.r, out.g, out.b, out.a);

        //bbs.setOutlineThickness(outline);
        //bbs.setOutlineColor(out);

        SDL_RenderDrawRect(rw, &shape);
        //rw.draw(bbs);
    }

    void draw_bounding_box(float x, float y, float w, float h,
                          Color fill = Color{0, 100, 0},
                          Color out  = Color{200, 100, 100}){

        return draw_bounding_box(Rectangle(x, y, w, h), fill, out);
    }


    // helpers
    // ------------------------------------------------------------------------
    Point render(Node* parent, const StringType& item, Point pos){
        log_trace("Rendering: ", item.string());

        float x = pos.x;
        float y = pos.y;
        float w = MAX(item.width(), style.width());
        float h = MAX(item.height(), style.height()) - outline;

        draw_bounding_box(x, y, w, h);

        if (!box_extracted)
            add_bounding_box(parent, x, y, w, h);

        item.render(rw, pos);
        pos.x += w;
        log_trace("Rendered");
        return pos;
    }

    Point new_line(Point pos, int indent){
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
    static Point run(RenderSurface& rw, RenderNode* expr, Point pos){
        log_trace("Initializing");
        RenderEngine eval(rw, pos);
        log_trace("Tree Renderer starting");
        return eval.traverse(expr, pos, 0);
    }

    Point function(Function* x, Point pos, int idt){
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
                log_trace("argument ", i + 1);
                pos = render(x, x->arg(i), pos);
                pos = render(x, style.get(':'), pos);
                pos = traverse(x->type->arg(i), pos, idt);
                pos = render(x, style.get(','), pos);
            }
            log_trace("argument ", n);
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

    Point new_line(Point pos){
        return Point(original_pos.x, pos.y + style.height());
    }

    Point indent(Point pos, int idt){
        return Point(original_pos.x + style.width() * 4 * idt , pos.y);
    }

    Point function_call(FunctionCall* x, Point pos, int idt){
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

    Point unary_call(UnaryCall* x, Point pos, int idt){
        if (!x->right)
            pos = render(x, x->name, pos);

        pos = traverse(x->arg(0), pos, idt);

        if (x->right)
            pos = render(x, x->name, pos);

        return pos;
    }

    Point binary_call(BinaryCall* x, Point pos, int idt){
        pos = traverse(x->lhs, pos, idt);
        pos = render(x, style.get(' '), pos);
        pos = render(x, x->name, pos);
        pos = render(x, style.get(' '), pos);
        pos = traverse(x->rhs, pos, idt);
        return pos;
    }

    Point borrow(Borrow* b, Point pos, int idt){
        return traverse(b->expr, pos, idt);
    }

    Point arrow(Arrow* x, Point pos, int idt){
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

    Point type(Type* x, Point pos, int){
        return render(x, x->name, pos);
    }

    Point builtin(Builtin* e, Point pos, int){
        return render(e, e->name, pos);
    }

    Point error(ErrorNode* e, Point pos, int){
        return render(e, e->message, pos);
    }

    Point value(Value* x, Point pos, int){
        std::string str = std::to_string(x->as<f64>());
        const char* c_str = str.c_str();
        StringType txt(c_str, Color(255, 255, 255));
        return render(x, txt, pos);
    }

    Point placeholder(Placeholder* x, Point pos, int){
        return render(x, x->name, pos);
    }
};

}
