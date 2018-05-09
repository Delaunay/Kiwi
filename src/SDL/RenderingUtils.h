#pragma once

#if defined(WIN32) && !defined(NOMINMAX)
#   define NOMINMAX
#endif


#include <algorithm>
#include "../Logging/Log.h"
#include "../AST/Root.h"

#include "StyleManager.h"


namespace kiwi {

    using RenderExpressionPtr = generic::Root<Expression<RenderTrait>>;
    using ASTExpressionPtr = generic::Root<Expression<ASTTrait>>;
    using RenderTypePtr = generic::Root<Type<RenderTrait>>;
    using ASTTypePtr = generic::Root<Type<ASTTrait>>;


    RenderTypePtr convert_type(ASTTypePtr expr);
    RenderExpressionPtr convert(ASTExpressionPtr expr);

    /* Render a string on the screen */
    class RenderingContext { 
    public:
        typedef SDL_Renderer* RenderSurface;
        typedef SDLString StringType;

        RenderingContext(RenderSurface& rw, Point2f pos, const StyleManager& style = StyleManager::style_manager()) :
            rw(rw), original_pos(pos), style(style)
        {}

        Point4f bounding_box(f32 x, f32 y, f32 w, f32 h) {
            top_box.x = std::min(x, top_box.x);
            top_box.y = std::min(y, top_box.y);
            top_box.width = std::max(x + w, top_box.width);
            top_box.height = std::max(y + h, top_box.height);
            return Point4f(x, y, w, h);
        }

        void draw_bounding_box(Point4f bb,
            Color fill = Color(0, 100, 0),
            Color out = Color(200, 100, 100)) {
            SDL_Rect shape = { i32(bb.x), i32(bb.y), i32(bb.width), i32(bb.height) };
            SDL_SetRenderDrawColor(rw, out.r, out.g, out.b, out.a);
            SDL_RenderDrawRect(rw, &shape);
        }

        void draw_bounding_box(f32 x, f32 y, f32 w, f32 h,
            Color fill = Color{ 0, 100, 0 },
            Color out = Color{ 200, 100, 100 }) {
            return draw_bounding_box(Point4f(x, y, w, h), fill, out);
        }

        // helpers
        // ------------------------------------------------------------------------
        std::tuple<Point2f, Point4f> render(const StringType& item, Point2f pos, f32 outline = 1) {
            log_trace("Rendering: ", item.string());

            f32 x = pos.x;
            f32 y = pos.y;
            f32 w = std::max(f32(item.width()), style.width());
            f32 h = std::max(f32(item.height()), style.height()) - outline;

            draw_bounding_box(x, y, w, h);

            item.render(rw, pos);
            pos.x += w;
            log_trace("Rendered");
            return std::make_tuple(pos, bounding_box(x, y, w, h));
        }

        Point2f new_line(Point2f pos, int32 indent) {
            pos.y += style.height();
            pos.x = original_pos.x + style.width() * indent;
            return pos;
        }

        Point2f new_line(Point2f pos) {
            return Point2f(original_pos.x, pos.y + style.height());
        }

        Point2f indent(Point2f pos, int32 idt) {
            return Point2f(original_pos.x + style.width() * 4 * idt, pos.y);
        }

        Point4f bound_box() { return top_box; }

        StyleManager const& style;

    private:
        RenderSurface&      rw;
        Point2f             original_pos = { 0, 0 };
        Point4f             top_box = { 10000, 10000, -10000, -10000 };
    };

}


