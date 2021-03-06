#ifndef KIWI_SDL_RENDERENGINE_HEADER_
#define KIWI_SDL_RENDERENGINE_HEADER_

#include "../../AST/Builder.h"
#include "../../Logging/Log.h"

//#include "../RenderTreeVisitor.h"
#include "../StyleManager.h"
//#include "../RenderingUtils.h"

namespace kiwi {

/*
 * Not Thread safe
 * /
class ExpressionRenderEngine : public RenderTreeVisitor<ExpressionRenderEngine, Point, Point, int> {
public:
    typedef SDL_Renderer* RenderSurface;
    typedef RenderTrait::StringType StringType;
    typedef SDL_Font* FontType;

    //using Builder = Builder<RenderTrait>;
    using ExpressionPtr = Builder::ExpressionPtr;
    using TypePtr = Builder::TypePtr;

    ExpressionRenderEngine(RenderSurface& rw, Point pos = { 0, 0 }) :
        rw(rw), original_pos(pos)
    {}

    static Point4f run(RenderSurface& rw, Expression<RenderTrait>* expr, Point pos) {
        log_trace("Initializing");
        ExpressionRenderEngine eval(rw, pos);
        log_trace("Tree Renderer starting");
        eval.parent = expr;
        eval.traverse(expr, pos, 0);
        return {0, 0, 0, 0};
    }

    RenderSurface&              rw;
    Point                       original_pos = { 0, 0 };
    Expression<RenderTrait>*    parent;

    enum class StringKind {
        Keyword,
        Placeholder,
        ArgumentName,
        Text,
        CallName,
        FunctionName,
        ErrorMessage,
        BuiltinName,
        String
    };

    template<typename T>
    StringDrawable make_string(const T& str, StringKind kind = StringKind::String) {
        switch (kind) {
        case StringKind::Keyword:       return RenderTrait::make_keyword(str);
        case StringKind::Placeholder:   return RenderTrait::make_placeholder_name(str);
        case StringKind::ArgumentName:  return RenderTrait::make_argument_name(str);
        case StringKind::Text:          return RenderTrait::make_text(str);
        case StringKind::CallName:      return RenderTrait::make_call_name(str);
        case StringKind::FunctionName:  return RenderTrait::make_function_name(str);
        case StringKind::ErrorMessage:  return RenderTrait::make_error_message(str);
        case StringKind::BuiltinName:   return RenderTrait::make_builtin_name(str);
        case StringKind::String:        return RenderTrait::make_string(str);
        }
    }

    Expression<RenderTrait>* parent;

    template<typename T>
    Point render_string(Expression<RenderTrait>* x, const T& str, Point pos, StringKind kind =
StringKind::String) { parent->insert_entity(make_string(str, kind));
    }

    Point function(Function<RenderTrait>* x, Point pos, int idt) {
        pos = render_string(x, "def", pos);
        pos = render_string(x, x->name, pos);
        pos = render_string(x, "(", pos);

        /*
        size_t na = 0;

        if (x->type != nullptr)
            na = x->type->args_size() - 1;

        size_t n = size_t(i32(x->args_size()) - 1);

        assert(n == na && "Arrow type size mistmatch");

        if (n >= 0) {
            for (size_t i = 0; i < n; ++i) {
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

        if (x->type != nullptr && x->type->return_type != nullptr) {
            pos = render_string(x, style.arrow(), pos);
            pos = render_type(x->type->return_type, pos, idt);
        }

        pos = render_string(x, style.get(':'), pos);
        pos = ctx.new_line(pos);
        pos = ctx.indent(pos, idt + 1);
        return render_expr(x->body, pos, idt + 1);* /
        return pos;
    }
};*/
}

#endif
