#pragma once

#include <string>

#include "SFMLNuklear.h"

#define KIWI_DEBUG
#include "../Debug.h"

namespace kiwi{

class Node{
public:
    struct nk_rect bounds = nk_rect(40, 10, 180, 220);
    struct nk_color color = nk_rgb(255, 0, 0);

    int id = 1;
    std::string name      = "test";
    float value           = 0;
    int input_count      = 2;
    int output_count     = 2;

    void draw(struct nk_context *ctx){
        //struct nk_panel *node = nullptr;

        nk_flags flags = NK_WINDOW_MOVABLE|NK_WINDOW_NO_SCROLLBAR|
                NK_WINDOW_BORDER|NK_WINDOW_TITLE;

        // Draw a Square
        if (nk_begin(ctx, name.c_str(), nk_rect(50, 50, 230, 250), flags))
        {
            /* ================= NODE CONTENT =====================*/
            nk_layout_row_dynamic(ctx, 25, 1);
            //node =  nk_window_get_panel(ctx);
            nk_button_color(ctx, color);

            color.r = (nk_byte) nk_propertyi(ctx, "#R:", 0, color.r, 255, 1,1);
            color.g = (nk_byte) nk_propertyi(ctx, "#G:", 0, color.g, 255, 1,1);
            color.b = (nk_byte) nk_propertyi(ctx, "#B:", 0, color.b, 255, 1,1);
            color.a = (nk_byte) nk_propertyi(ctx, "#A:", 0, color.a, 255, 1,1);

            /* ====================================================*/
            //nk_group_end(ctx);
        }


        struct nk_command_buffer *canvas = nk_window_get_canvas(ctx);
        struct nk_panel *node =  nk_window_get_panel(ctx);

        //printd(node);

        float space;
        struct nk_rect bounds;
        bounds = nk_layout_space_rect_to_local(ctx, node->bounds);
        this->bounds = bounds;
        space = bounds.h / (float)((output_count) + 1);


        for (int n = 0; n < output_count; ++n) {
            struct nk_rect circle;
            circle.x = node->bounds.x + node->bounds.w-4;
            circle.y = node->bounds.y + space * (float)(n+1);
            circle.w = 8; circle.h = 8;
            nk_fill_circle(canvas, circle, nk_rgb(100, 100, 100));

            /* start linking process *
            if (nk_input_has_mouse_click_down_in_rect(in, NK_BUTTON_LEFT,
                                                      circle, nk_true)) {
                nodedit->linking.active = nk_true;
                nodedit->linking.node = it;
                nodedit->linking.input_id = it->ID;
                nodedit->linking.input_slot = n;
            }*/

            /* draw curve from linked node slot to mouse position *
            if (nodedit->linking.active && nodedit->linking.node == it &&
                nodedit->linking.input_slot == n) {
                struct nk_vec2 l0 = nk_vec2(circle.x + 3, circle.y + 3);
                struct nk_vec2 l1 = in->mouse.pos;
                nk_stroke_curve(canvas, l0.x, l0.y, l0.x + 50.0f, l0.y,
                    l1.x - 50.0f, l1.y, l1.x, l1.y, 1.0f, nk_rgb(100, 100, 100));
            }*/
        }

        /* input connector */
        space = node->bounds.h / (float)((input_count) + 1);
        for (int n = 0; n < input_count; ++n) {
            struct nk_rect circle;
            circle.x = node->bounds.x-4;
            circle.y = node->bounds.y + space * (float)(n+1);
            circle.w = 8; circle.h = 8;
            nk_fill_circle(canvas, circle, nk_rgb(100, 100, 100));
            /*
            if (nk_input_is_mouse_released(in, NK_BUTTON_LEFT) &&
                nk_input_is_mouse_hovering_rect(in, circle) &&
                nodedit->linking.active && nodedit->linking.node != it)
            {
                nodedit->linking.active = nk_false;
                node_editor_link(nodedit, nodedit->linking.input_id,
                    nodedit->linking.input_slot, it->ID, n);
            }*/
        }

        nk_end(ctx);
    }


};


}
