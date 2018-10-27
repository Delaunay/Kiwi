#include "Drawable.h"

namespace kiwi {
Drawable::~Drawable() {}

String const Drawable::drawable_name() { return "Drawable"; }

// Event Handling
void Drawable::handle_mouse_motion(SDL_MouseMotionEvent) {}

void Drawable::handle_mouse_wheel(SDL_MouseWheelEvent) {}

void Drawable::handle_text_editing(SDL_TextEditingEvent) {}

void Drawable::handle_text_input(SDL_TextInputEvent) {}

void Drawable::handle_window(SDL_WindowEvent) {}

void Drawable::handle_mouse_button(bool, SDL_MouseButtonEvent) {}

void Drawable::handle_keyboard(bool, SDL_KeyboardEvent) {}

} // namespace kiwi
