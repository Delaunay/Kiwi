#ifndef KIWI_EDITABLE_STRING_HEADER
#define KIWI_EDITABLE_STRING_HEADER

#include "StringDrawable.h"

namespace kiwi {

    class EditableString : public StringDrawable {
    public:
        EditableString(const std::string& str, SDL_Color color = default_color(), SDL_Font* font = default_font()) :
            StringDrawable(str, color, font)
        {}

        Point draw(SDL_Renderer *renderer, Point oposition) override {
            log_trace("Draw EditableString");
            auto pos = off_screen_draw(renderer, {0, 0});
            auto target = SDL_GetRenderTarget(renderer);
            SDL_Texture* cache = texture();
            CHECK(SDL_SetRenderTarget(renderer, cache));

            if (!_cursor_drawed) {
                SDL_Rect cursor = { glyph_width() * (1 + _cursor_position_char), 3, 2 , glyph_height() - 6 };
                SDL_SetRenderDrawColor(renderer, 155, 155, 255, 125);
                SDL_RenderFillRect(renderer, &cursor);
                _cursor_drawed = true;
            }

            CHECK(SDL_SetRenderTarget(renderer, target));
            SDL_Rect box;
            box.x = i32(oposition.x);
            box.y = i32(oposition.y); // 
            box.w = i32(size().x);
            box.h = i32(size().y);

            // Clean Area
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, &box);

            CHECK(SDL_RenderCopy(renderer, cache, nullptr, &box));
            return { oposition.x + size().x, oposition.y };
        }


        void handle_mouse_wheel(SDL_MouseWheelEvent event) override {}

        void handle_window(SDL_WindowEvent wevent) override {}

        void handle_mouse_motion(SDL_MouseMotionEvent event) override {
        
        }

        // Returns the String character Index closed to the given Position(x, y) 
        i32 get_cursor_position(i32 x, i32 y) {
            i32 pos = x - position().x;
            i32 char_pos = pos / glyph_width();
            f32 i = f32(pos) / f32(glyph_width()) - pos / glyph_width();
            if (i < 0.5) char_pos -= 1;
            return char_pos;
        }

        void handle_mouse_button(bool up, SDL_MouseButtonEvent event) override {
            if(!up) {
                _start_position = { event.x, event.y };
                _selecting = true;
            } else {
                Point2i end_position = { event.x, event.y };
                _selecting = false;
                _selected = true;

                std::cout << "Started (x:" << _start_position.x << ", " << _start_position.y << ")" << std::endl;
                std::cout << "End (x:" << end_position.x << ", " << end_position.y << ")" << std::endl;
            }

            if (event.clicks == 1 && up && event.x > position().x) {
                _cursor_position_char = get_cursor_position(event.x, event.y);
                log_trace("Position: ", _cursor_position_char);
                reset_cache();
            }
            else if (event.clicks == 2) {
                log_info("Double Click");
                _selected = true;
                _start_position = { i32(position().x), i32(position().y) };
                _end_position = { i32(position().x + size().x), i32(position().y + size().y) };

            }
        }

        void reset_cache() {
            _cursor_drawed = false;
            StringDrawable::reset_cache();
            _selected = false;
        }

        void handle_keyboard(bool up, SDL_KeyboardEvent event) override {
            if (!up) {
                switch (event.keysym.sym) {
                case SDLK_LEFT:{
                    _cursor_position_char = std::max(-1, _cursor_position_char - 1);
                    reset_cache();
                    break;
                }
                case SDLK_RIGHT:{
                    _cursor_position_char = std::min(string_size(), _cursor_position_char + 1);
                    reset_cache();
                    break;
                }
                case SDLK_BACKSPACE:{
                    remove_char();
                    break;
                }

                case SDLK_c: {
                    if (SDL_GetModState() & KMOD_CTRL) {
                        log_info("Setting clipboard: ", selected());
                        SDL_SetClipboardText(selected());
                        break;
                    }
                }
                case SDLK_v: {
                    if (SDL_GetModState() & KMOD_CTRL) {
                        log_info("Setting from clipboard");
                        insert_text(SDL_GetClipboardText());
                        break;
                    }
                }
                }
            }
            log_trace("New Pos (char: ", _cursor_position_char, ") (byte: ", get_byte_loc(string(), _cursor_position_char + 1), ")");
        }

        void handle_text_editing(SDL_TextEditingEvent event) override {}

        void handle_text_input(SDL_TextInputEvent event) override {
            log_trace("text_input");

            if (!(SDL_GetModState() & KMOD_CTRL && 
                (event.text[0] == 'c' || event.text[0] == 'C') &&
                (event.text[0] == 'v' || event.text[0] == 'V'))){ 
                insert_text(event.text);
            }
        }

        void insert_text(char const* text) {
            log_info();
            String str = string();
            set_string(str.insert(get_byte_loc(string(), _cursor_position_char + 1), text));
            _cursor_position_char += utf8_strlen(text);
            _cursor_drawed = false;
            log_info();
        }

        char const* selected() const {
            return c_string();
        }

        void remove_char() {
            if (_cursor_position_char > -1){
                String str = string();
                set_string(str.erase(_cursor_position_char, get_char_byte_size(str, _cursor_position_char)));
                _cursor_position_char -= 1;
                reset_cache();
            }
        }

    private:
        bool _editing = false;
        bool _cursor_drawed = false;
        i32 _cursor_position_char = 0;

        // Mouse Select
        bool    _selecting = false;
        bool    _selected  = false;
        Point2i _start_position = { 0, 0 };
        Point2i _end_position   = { 0, 0 };
    };
}

#endif