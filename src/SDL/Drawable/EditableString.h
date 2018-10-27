#ifndef KIWI_EDITABLE_STRING_HEADER
#define KIWI_EDITABLE_STRING_HEADER

#include "StringDrawable.h"

namespace kiwi {

class EditableString : public StringDrawable {
  public:
    EditableString(
        const std::string &str,
        const StringStyle &style = StyleManager::style_manager().get_style(MappedStyle::Default)) :
        StringDrawable(str, style) {}

    // Unset Texture, Telling us to redraw the string next time
    void reset_cache();

    Point draw(SDL_Renderer *renderer, Point oposition) override;

    // Returns the String character Index closed to the given Position(x, y)
    i32 get_cursor_position(i32 x, i32 y);

    void remove_char();

    // insert at _cursor_position_char
    void insert_text(char const *text);

    // return text between _start_position & _end_position
    char const *selected() const { return c_string(); }

    // Event Handler
    void handle_mouse_button(bool up, SDL_MouseButtonEvent event) override;

    void handle_mouse_wheel(SDL_MouseWheelEvent) override;

    void handle_window(SDL_WindowEvent) override;

    void handle_mouse_motion(SDL_MouseMotionEvent) override;

    void handle_keyboard(bool up, SDL_KeyboardEvent event) override;

    void handle_text_editing(SDL_TextEditingEvent) override;

    void handle_text_input(SDL_TextInputEvent event) override;

  private:
    bool _cursor_drawn        = false;
    i64 _cursor_position_char = 0;

    // Mouse Select
    bool _selecting         = false;
    bool _selected          = false;
    Point2i _start_position = {0, 0};
    Point2i _end_position   = {0, 0};
};
} // namespace kiwi

#endif
