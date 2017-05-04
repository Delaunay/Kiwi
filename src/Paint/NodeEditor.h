#pragma once

#include "SFMLNuklear.h"

namespace kiwi{

class NodeEditor{


    int draw(struct nk_context *ctx){
        nk_flags flags = NK_WINDOW_BORDER|NK_WINDOW_NO_SCROLLBAR|
                NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE;

        if (nk_begin(ctx, "NodeEdit", nk_rect(0, 0, 800, 600), flags))
        {
            canvas = nk_window_get_canvas(ctx);
            total_space = nk_window_get_content_region(ctx);
            struct nk_rect size = nk_layout_space_bounds(ctx);

            nk_layout_space_begin(ctx, NK_STATIC, total_space.h, nodedit->node_count);

            if (show_grid) grid(size);


            // for each node


            // draw links

            // context menu


        }
        nk_end(ctx);
        return !nk_window_is_closed(ctx, "NodeEdit");
    }

    void grid(struct nk_rect size){

            const float grid_size = 32.0f;
            const struct nk_color grid_color = nk_rgb(50, 50, 50);

            float x = (float)fmod(size.x - scrolling.x, grid_size);
            for (; x < size.w; x += grid_size)
                nk_stroke_line(canvas,
                               x + size.x, size.y,
                               x + size.x, size.y + size.h, 1.0f, grid_color);

            float y = (float)fmod(size.y - scrolling.y, grid_size);
            for (; y < size.h; y += grid_size)
                nk_stroke_line(canvas,
                               size.x, y + size.y,
                               size.x + size.w, y + size.y, 1.0f, grid_color);
    }


    struct nk_command_buffer *canvas;
    struct nk_rect total_space;

    struct node node_buf[32];
    struct node_link links[64];

    struct node *begin;
    struct node *end;

    struct nk_rect bounds;
    struct node *selected;

    struct nk_vec2 scrolling;
    struct node_linking linking;

    int node_count;
    int link_count;

    int show_grid;
    int initialized;
};

}
