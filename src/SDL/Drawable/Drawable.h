#ifndef KIWI_DRAWABLE_HEADER
#define KIWI_DRAWABLE_HEADER

#include "../Point.h"
#include "../SDLHelper.h"

namespace kiwi {
class Drawable {
  public:
    virtual ~Drawable();

    virtual Point draw(SDL_Renderer *renderer, Point position) = 0;

    virtual String const drawable_name();

    // Event Handling
    virtual void handle_mouse_motion(SDL_MouseMotionEvent);

    virtual void handle_mouse_wheel(SDL_MouseWheelEvent);

    virtual void handle_text_editing(SDL_TextEditingEvent);

    virtual void handle_text_input(SDL_TextInputEvent);

    virtual void handle_window(SDL_WindowEvent);

    virtual void handle_mouse_button(bool up, SDL_MouseButtonEvent);

    virtual void handle_keyboard(bool down, SDL_KeyboardEvent);

    virtual Point size() const = 0;

    Point position() const { return _position; }

    void set_position(Point pos) { _position = pos; }

    bool collide(int x, int y) {
        if(x > position().x && y > position().y && position().x + size().x > x &&
           position().y + size().y > y) {
            return true;
        }
        return false;
    }

  private:
    Point _position;
};

} // namespace kiwi

#endif
