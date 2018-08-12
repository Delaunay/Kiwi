#ifndef KIWI_STYLE_MANAGER_HEADER
#define KIWI_STYLE_MANAGER_HEADER

//#include "Drawable/RenderNode.h"
#include "Drawable/StringDrawable.h"

namespace kiwi {

struct StyleManager {
  private:
    StyleManager() { log_info("Style Manager was created"); }

    static StyleManager init() {
        log_info("Intitialized");
        return StyleManager();
    }

  public:
    using StringType = StringDrawable;
    typedef SDL_Font *FontType;

    static StyleManager &style_manager() {
        static StyleManager style = init();
        return style;
    }

    const StringType &get(char c) const {
        switch(c) {
        case '(':
            return fun_open;
        case ')':
            return fun_close;
        case ':':
            return fun_col;
        case ' ':
            return _space;
        case ',':
            return fun_com;
        default:
            log_error("Requesting an non-predef");
            return _space;
        }
    }

    const StringType &def() const { return fun_def; }
    const StringType &arrow() const { return _arrow; }
    const StringType &space() const { return _space; }

    f32 width() const { return glyph_width; }
    f32 height() const { return glyph_height; }

    f32 width(const StringType &txt) const { return f32(txt.size().x); }
    f32 height(const StringType &) const { return f32(glyph_height); }

    Point2f size(const StringType &txt) const { return Point(width(txt), height(txt)); }

    //** /
    const StringType &open_parenthesis() const { return fun_open; }
    const StringType &close_parenthesis() const { return fun_close; }

    template <typename T> static StringType make_text(const T &str, const SDL_Color &color) {
        StringType txt(str, color, StringType::default_font());
        return txt;
    }

    template <typename T> StringType text(const T &str) {
        log_info("Making text ");
        return make_text(str, {25, 25, 25});
    }

    template <typename T> StringType keyword(const T &str) {
        log_info("Making keyword ");
        auto x = make_text(str, {50, 50, 50});
        log_info("Making kedn ");
        return x;
    } //*/

    const FontType &font() const { return _font; }
    uint32 font_size() const { return _font_size; }

  private:
    FontType _font    = StringDrawable::default_font();
    uint32 _font_size = 12;

    /**/
    StringType fun_def   = keyword("def ");
    StringType fun_open  = text("(");
    StringType fun_close = text(")");
    StringType fun_col   = text(": ");
    StringType fun_com   = text(", ");
    StringType _space    = text(" ");
    StringType _arrow    = text(u8" \u2192 "); //*/

    f32 glyph_width  = f32(_space.size().x);     // space.getCharacterSize();
    f32 glyph_height = f32(_space.size().y + 1); // space.getGlobalBounds().height;
};

} // namespace kiwi

#endif
