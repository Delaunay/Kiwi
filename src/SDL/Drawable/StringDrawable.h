#ifndef KIWI_STRINGDRAWABLE_HEADER
#define KIWI_STRINGDRAWABLE_HEADER

#define SDL_MAIN_HANDLED
#include <SDL.h>

#ifdef __linux__
#include <SDL2/SDL_ttf.h>
#else
#include <SDL_ttf.h>
#endif

#include "../Configuration.h"
#include "../Point.h"
#include "../SDLHelper.h"
#include "../StyleManager.h"
#include "Drawable.h"

#undef log_trace
#define log_trace(...)

#include <algorithm>

namespace kiwi {

// Create an Offscreen Rendering Target (Texture)
// Render to Surface if needed
// Draw to Rendering Target (Origin: (0, 0))
// Copy Rendering Target to Screen at the given position

class StringDrawable : public Drawable {
  public:
    StringDrawable(
        const std::string &str,
        const StringStyle &style = StyleManager::style_manager().get_style(MappedStyle::Default));

    void string_to_surface(bool init = false);

    void reset_cache() {
        log_trace();
        _texture = nullptr;
    }

    void underline(SDL_Renderer *renderer, int tickness, SDL_Color color) const;

    void strike(SDL_Renderer *renderer, int tickness, SDL_Color color) const;

    void drawHorizontalLine(int y, SDL_Renderer *renderer, int tickness, SDL_Color color) const;

    Point off_screen_draw(SDL_Renderer *renderer, Point position);

    Point draw(SDL_Renderer *renderer, Point position) override;

    Point size() const override { return _size; }

    // Blended, _string, _font, _style.render_color
    SDL_Surface *render_utf8();

    void set_string(String const &str);

    const char *c_string() const { return _string.c_str(); }

    const String &string() const { return _string; }

    std::size_t string_size() const { return _string.size(); }

    SDL_Texture *texture() const { return _texture.get(); }

    int length() const { return utf8_strlen(string()); }

    // from http://www.zedwood.com/article/cpp-utf8-strlen-function
    static int utf8_strlen(String const &str);

    static i64 get_char_byte_size(const String &str, u64 byte_pos);

    static i64 get_byte_loc(const String &str, i64 char_pos);

    i32 glyph_width() const;
    i32 glyph_height() const;

    // FIXME: find where this is required
    operator const String &() const { return _string; }

    String const drawable_name() override { return String("StringDrawable(\"") + _string + "\")"; }

  private:
    int mutable _count = 0;
    Point _size;
    String _string;
    SurfacePtr _surface;
    mutable TexturePtr _texture;
    StringStyle const &_style;
};
} // namespace kiwi

#endif
