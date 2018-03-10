#pragma once

#ifndef WIN32
#   include <algorithm>
#   define MIN(x, y) std::min(float(x), float(y))
#   define MAX(x, y) std::max(float(x), float(y))
#else
#   define MIN(x, y) (((x) > (y)) ? (y) : (x))
#   define MAX(x, y) (((x) < (y)) ? (y) : (x))
#endif

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

        RenderingContext(RenderSurface& rw, Point pos, const StyleManager& style = StyleManager::style_manager()) :
            rw(rw), original_pos(pos), style(style)
        {}

        Rectangle bounding_box(float x, float y, float w, float h) {
            top_box.x = MIN(x, top_box.x);
            top_box.y = MIN(y, top_box.y);
            top_box.width = MAX(x + w, top_box.width);
            top_box.height = MAX(y + h, top_box.height);
            return Rectangle(x, y, w, h);
        }

        void draw_bounding_box(Rectangle bb,
            Color fill = Color(0, 100, 0),
            Color out = Color(200, 100, 100)) {
            SDL_Rect shape = { bb.x, bb.y, bb.width, bb.height };
            SDL_SetRenderDrawColor(rw, out.r, out.g, out.b, out.a);
            SDL_RenderDrawRect(rw, &shape);
        }

        void draw_bounding_box(float x, float y, float w, float h,
            Color fill = Color{ 0, 100, 0 },
            Color out = Color{ 200, 100, 100 }) {
            return draw_bounding_box(Rectangle(x, y, w, h), fill, out);
        }

        // helpers
        // ------------------------------------------------------------------------
        std::tuple<Point, Rectangle> render(const StringType& item, Point pos, float outline = 1) {
            log_trace("Rendering: ", item.string());

            float x = pos.x;
            float y = pos.y;
            float w = MAX(item.width(), style.width());
            float h = MAX(item.height(), style.height()) - outline;

            draw_bounding_box(x, y, w, h);

            item.render(rw, pos);
            pos.x += w;
            log_trace("Rendered");
            return std::make_tuple(pos, bounding_box(x, y, w, h));
        }

        Point new_line(Point pos, int indent) {
            pos.y += style.height();
            pos.x = original_pos.x + style.width() * indent;
            return pos;
        }

        Point new_line(Point pos) {
            return Point(original_pos.x, pos.y + style.height());
        }

        Point indent(Point pos, int idt) {
            return Point(original_pos.x + style.width() * 4 * idt, pos.y);
        }

        Rectangle bound_box() { return top_box; }

        StyleManager const& style;

    private:
        RenderSurface&      rw;
        Point               original_pos = { 0, 0 };
        Rectangle           top_box = { 10000, 10000, -10000, -10000 };
    };

}


