#pragma once


#include "../AST/Builder.h"
#include "../Logging/Log.h"

#include "RenderTreeVisitor.h"
#include "StyleManager.h"
#include "RenderingUtils.h"

namespace kiwi{

    class TypeRenderEngine : public RenderTypeVisitor<TypeRenderEngine, Point, Point, int> {
    public:
        typedef SDL_Renderer* RenderSurface;
        typedef SDLString StringType;
        typedef SDL_Font* FontType;

        using Builder = Builder<RenderTrait>;
        using ExpressionPtr = Builder::ExpressionPtr;
        using TypePtr = Builder::TypePtr;

        TypeRenderEngine(RenderingContext& ctx, bool box_extracted) :
            ctx(ctx), box_extracted(box_extracted)
        {}

        Point render_string(Type<RenderTrait>* x, const StringType& str, Point pos) {
            Point4f bbox(0, 0, 0, 0);
            std::tie(pos, bbox) = ctx.render(str, pos);

            if (!box_extracted)
                x->bound_box = bbox;

            return pos;
        }

        Point arrow(Arrow<RenderTrait>* x, Point pos, int idt) {
            pos = render_string(x, ctx.style.get('('), pos);

            int n = int(x->args.size()) - 1;
            for (int i = 0; i < n; ++i) {
                pos = traverse(x->arg(i), pos, idt);
                pos = render_string(x, ctx.style.get(','), pos);
            }
            pos = traverse(x->arg(n), pos, idt);
            pos = render_string(x, ctx.style.get(')'), pos);
            pos = render_string(x, ctx.style.arrow(), pos);
            pos = traverse(x->return_type, pos, idt);
            return pos;
        }

        Point builtin(Builtin<RenderTrait>* e, Point pos, int) {
            return render_string(e, e->name, pos);
        }

        Point error(ErrorType<RenderTrait>* e, Point pos, int) {
            return render_string(e, e->message, pos);
        }

        Point unkown(Unknown<RenderTrait>* e, Point pos, int) {
            return pos;
        }

        Point record(Record<RenderTrait>* e, Point pos, int) {
            return pos;
        }

        Point error(ErrorNode<RenderTrait>* e, Point pos, int) {
            return pos;
        }

        RenderingContext& ctx;
        bool box_extracted;
    };

class ExpressionRenderEngine: public RenderTreeVisitor<ExpressionRenderEngine, Point, Point, int>{
public:
    typedef SDL_Renderer* RenderSurface;
    typedef SDLString StringType;
    typedef SDL_Font* FontType;

    using Builder = Builder<RenderTrait>;
    using ExpressionPtr = Builder::ExpressionPtr;
    using TypePtr = Builder::TypePtr;

    ExpressionRenderEngine(RenderSurface& rw, Point pos = {0, 0}):
        rw(rw), original_pos(pos), box_extracted(false), ctx(rw, pos), type_engine(ctx, box_extracted)
    { }

    RenderSurface&              rw;
    Point                       original_pos = { 0, 0 };
    bool                        box_extracted = false;
    RenderingContext            ctx;
    TypeRenderEngine            type_engine;

    std::vector<Expression<RenderTrait>*> bounding_boxes;
    const StyleManager&                   style = StyleManager::style_manager();
    Point4f                               top_box = {10000, 10000, -10000, -10000};
    Expression<RenderTrait>*              current_expression = nullptr;
    
    // Check if `render` inputs have changed
    // if so we need to recompute the bouding boxes and
    // update positions
    bool has_input_changed(Expression<RenderTrait>* expr, Point pos){
        return (current_expression != expr || pos != original_pos);
    }

    void render(Expression<RenderTrait>* expr, Point pos){
        if (has_input_changed(expr, pos)){
            current_expression = expr;
            original_pos = pos;
            bounding_boxes.clear();
            box_extracted = false;
            type_engine.box_extracted = false;
        } else if (box_extracted){
            ctx.draw_bounding_box(top_box,
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
        type_engine.box_extracted = true;
    }

    Point render_expr(Expression<RenderTrait>* tp, Point pos, int idt) {
        return traverse(tp, pos, idt);
    }

    Point render_type(Type<RenderTrait>* tp, Point pos, int idt) {
        return type_engine.traverse(tp, pos, idt);
    }

    static ExpressionPtr make_sqr(){
        ExpressionPtr x    = Builder::placeholder("x");
        ExpressionPtr body = Builder::mult(x, Builder::borrow(x));

        TypePtr dbl = Builder::builtin("double");
        TypePtr fun_type = Builder::arrow({dbl.get()}, dbl.get());

        ExpressionPtr sqr  = Builder::function("sqr", body, fun_type);
        Expression<RenderTrait>* node = sqr.get();

        // generic::Function<RenderNode>*
        Function<RenderTrait>* f = static_cast<Function<RenderTrait>*>(node);
        f->add_arg("x");

        return sqr;
    }

    // ------------------------------------------------------------------------
    static std::vector<Expression<RenderTrait>*> run(RenderSurface& rw, Expression<RenderTrait>* expr, Point pos){
        log_trace("Initializing");
        ExpressionRenderEngine eval(rw, pos);
        log_trace("Tree Renderer starting");
		eval.render_expr(expr, pos, 0);
		return eval.bounding_boxes;
    }

    Point render_string(Expression<RenderTrait>* x, const StringType& str, Point pos) {
        Point4f bbox(0, 0, 0, 0);
        std::tie(pos, bbox) = ctx.render(str, pos);     

        if (!box_extracted) 
            x->bound_box = bbox;

        return pos;
    }

    Point function(Function<RenderTrait>* x, Point pos, int idt){
        pos = render_string(x, style.def(), pos);     
        pos = render_string(x, x->name, pos);         
        pos = render_string(x, style.get('('), pos);  

        size_t na = 0;

        if (x->type != nullptr)
            na = x->type->args_size() - 1;

        size_t n = size_t(i32(x->args_size()) - 1);

        assert(n == na && "Arrow type size mistmatch");

        if (n >= 0){
            for(size_t i = 0; i < n; ++i){
                log_trace("argument ", i + 1);
                pos = render_string(x, x->arg(i), pos);
                pos = render_string(x, style.get(':'), pos);
                pos = render_type(x->type->arg(i), pos, idt);
                pos = render_string(x, style.get(','), pos);
            }
            log_trace("argument ", n);
            pos = render_string(x, x->arg(n), pos);
            pos = render_string(x, style.get(':'), pos); 
            pos = render_type(x->type->arg(na), pos, idt);
        }

        pos = render_string(x, style.get(')'), pos);

        if (x->type != nullptr && x->type->return_type != nullptr){
            pos = render_string(x, style.arrow(), pos);
            pos = render_type(x->type->return_type, pos, idt);
        }

        pos = render_string(x, style.get(':'), pos);
        pos = ctx.new_line(pos);
        pos = ctx.indent(pos, idt + 1);
        return render_expr(x->body, pos, idt + 1);
    }

    Point function_call(FunctionCall<RenderTrait>* x, Point pos, int idt){
        pos = render_string(x, x->name, pos);
        pos = render_string(x, style.get('('), pos);

        size_t n = x->args_size() - 1;

        for(int i = 0; i < n; ++i){
            pos = render_expr(x->arg(i), pos, idt);
            pos = render_string(x, style.get(','), pos);
        }

        pos = render_expr(x->arg(n), pos, idt);
        pos = render_string(x, style.get(')'), pos);
        return pos;
    }

    Point unary_call(UnaryCall<RenderTrait>* x, Point pos, int idt){
        if (!x->right) {
            pos = render_string(x, x->name, pos);
            pos = render_string(x, style.get(' '), pos);
        }

        pos = render_expr(x->arg(0), pos, idt);

        if (x->right)
            pos = render_string(x, x->name, pos);

        return pos;
    }

    Point binary_call(BinaryCall<RenderTrait>* x, Point pos, int idt){
        pos = render_expr(x->lhs, pos, idt);
        pos = render_string(x, style.get(' '), pos);
        pos = render_string(x, x->name, pos);
        pos = render_string(x, style.get(' '), pos);
        pos = render_expr(x->rhs, pos, idt);
        return pos;
    }

    Point borrow(Borrow<RenderTrait>* b, Point pos, int idt){
        return render_expr(b->expr, pos, idt);
    }

    Point error(ErrorNode<RenderTrait>* e, Point pos, int){
        return render_string(e, e->message, pos);
    }

    Point value(Value<RenderTrait>* x, Point pos, int){
        std::string str = std::to_string(x->as<f64>());
        const char* c_str = str.c_str();
        StringType txt(c_str, Color(255, 255, 255));
        return render_string(x, txt, pos);
    }

    Point placeholder(Placeholder<RenderTrait>* x, Point pos, int){
        return render_string(x, x->name, pos);
    }
};

}
