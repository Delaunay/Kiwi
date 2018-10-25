#ifndef KIWI_STYLE_MANAGER_HEADER
#define KIWI_STYLE_MANAGER_HEADER

#ifdef __linux__
#include <SDL2/SDL_ttf.h>
#else
#include <SDL_ttf.h>
#endif

#include "../Logging/Log.h"
#include "Point.h"
#include "SDLHelper.h"

namespace kiwi {
typedef TTF_Font SDL_Font;

enum RenderingType { Solid, Blended, Shaded };

struct StringStyle {
    StringStyle(SDL_Color render_color = {255, 255, 255, 0}, bool highlight = false,
                SDL_Color highlight_color = {255, 255, 255, 0}, bool underline = false,
                u8 underline_tickness = 2, SDL_Color underline_color = {255, 255, 255, 0},
                SDL_Font *font = nullptr, RenderingType rendering_type = Shaded) :
        render_color(render_color),
        highlight(highlight), highlight_color(highlight_color), underline(underline),
        underline_tickness(underline_tickness), underline_color(underline_color), font(font),
        rendering_type(rendering_type) {}

    SDL_Color render_color       = {255, 255, 255, 0};
    bool highlight               = false;
    SDL_Color highlight_color    = {255, 255, 255, 0};
    bool underline               = false;
    u8 underline_tickness        = 2;
    SDL_Color underline_color    = {255, 255, 255, 0};
    SDL_Font *font               = nullptr;
    RenderingType rendering_type = Shaded;
};

enum class MappedStyle {
    Default = 0,
    Keyword,
    TypeName,
    Placeholder,
    FunctionName,
    ErrorMessage,
    Builtin,
    ArgumentName,
    Comment
};

struct StyleManager {
  private:
    StyleManager() { log_info("Style Manager was created"); }
    StyleManager(StyleManager const &cp) = default;

    static StyleManager init() {
        log_info("Intitialized");
        StyleManager m;
        TTF_SizeUTF8(m._font, " ", &m.glyph_width, &m.glyph_height);
        return m;
    }

    ~StyleManager() {}

  public:
    typedef SDL_Font *FontType;

    static StyleManager &style_manager() {
        static StyleManager style = init();
        return style;
    }

    f32 width() const { return glyph_width; }
    f32 height() const { return glyph_height; }

    const FontType &font() const { return _font; }
    uint32 font_size() const { return _font_size; }

    static SDL_Font *default_font() {
        log_trace();
        static SDL_Font *font = CHECK(TTF_OpenFont(HOME "DejaVuSansMonoPowerline.ttf", 24));
        return font;
    }

    static SDL_Color default_color() {
        log_trace();
        return SDL_Color{255, 255, 255, 0};
    }

    StringStyle const &get_style(MappedStyle name) {
        switch(name) {
        case MappedStyle::Default:
            return default_style;
        case MappedStyle::Keyword:
            return keyword;
        case MappedStyle::TypeName:
            return type_name;
        case MappedStyle::Placeholder:
            return placeholder;
        case MappedStyle::FunctionName:
            return function_name;
        case MappedStyle::ErrorMessage:
            return error_message;
        case MappedStyle::Builtin:
            return builtin_name;
        case MappedStyle::ArgumentName:
            return argument_name;
        case MappedStyle::Comment:
            return comment;
        default:
            return default_style;
        }
    }

  public:
    // ==================================================================================
    // UTF8 Char to use
    String arrow = u8" \u2192 "; // ->

    // ==================================================================================
    // Global Style Constant
    Color background_color = {35, 35, 35, 0};

    // https://stackoverflow.com/questions/18731707/why-does-c11-not-support-designated-initializer-lists-as-c99
    // TLDR somehow it is hard
    StringStyle default_style = StringStyle(
        /*.render_color       = */ {255, 255, 255, 0},
        /*.highlight          = */ false,
        /*.highlight_color    = */ {255, 255, 255, 0},
        /*.underline          = */ false,
        /*.underline_tickness = */ 2,
        /*.underline_color    = */ {255, 255, 255, 0},
        /*.font               = */ default_font(),
        /*.rendering_type     = */ Shaded);
    StringStyle keyword = StringStyle(
        /*.render_color       = */ {255, 155, 155, 0},
        /*.highlight          = */ false,
        /*.highlight_color    = */ {155, 255, 255, 0},
        /*.underline          = */ false,
        /*.underline_tickness = */ 2,
        /*.underline_color    = */ {255, 255, 255, 0},
        /*.font               = */ default_font(),
        /*.rendering_type     = */ Shaded);
    StringStyle type_name = StringStyle(
        /*.render_color       = */ {255, 255, 255, 0},
        /*.highlight          = */ false,
        /*.highlight_color    = */ {255, 255, 255, 0},
        /*.underline          = */ false,
        /*.underline_tickness = */ 2,
        /*.underline_color    = */ {255, 255, 255, 0},
        /*.font               = */ default_font(),
        /*.rendering_type     = */ Shaded);
    StringStyle placeholder = StringStyle(
        /*.render_color       = */ {255, 155, 255, 0},
        /*.highlight          = */ false,
        /*.highlight_color    = */ {255, 255, 255, 0},
        /*.underline          = */ false,
        /*.underline_tickness = */ 2,
        /*.underline_color    = */ {255, 255, 255, 0},
        /*.font               = */ default_font(),
        /*.rendering_type     = */ Shaded);
    StringStyle function_name = StringStyle(
        /*.render_color       = */ {155, 255, 255, 0},
        /*.highlight          = */ false,
        /*.highlight_color    = */ {255, 255, 255, 0},
        /*.underline          = */ false,
        /*.underline_tickness = */ 2,
        /*.underline_color    = */ {255, 255, 255, 0},
        /*.font               = */ default_font(),
        /*.rendering_type     = */ Shaded);
    StringStyle error_message = StringStyle(
        /*.render_color       = */ {255, 255, 255, 0},
        /*.highlight          = */ false,
        /*.highlight_color    = */ {255, 255, 255, 0},
        /*.underline          = */ false,
        /*.underline_tickness = */ 2,
        /*.underline_color    = */ {255, 255, 255, 0},
        /*.font               = */ default_font(),
        /*.rendering_type     = */ Shaded);
    StringStyle builtin_name = StringStyle(
        /*.render_color       = */ {155, 255, 155, 0},
        /*.highlight          = */ false,
        /*.highlight_color    = */ {255, 255, 255, 0},
        /*.underline          = */ false,
        /*.underline_tickness = */ 2,
        /*.underline_color    = */ {255, 255, 255, 0},
        /*.font               = */ default_font(),
        /*.rendering_type     = */ Shaded);
    StringStyle argument_name = StringStyle(
        /*.render_color       = */ {155, 155, 155, 0},
        /*.highlight          = */ false,
        /*.highlight_color    = */ {255, 255, 255, 0},
        /*.underline          = */ false,
        /*.underline_tickness = */ 2,
        /*.underline_color    = */ {255, 255, 255, 0},
        /*.font               = */ default_font(),
        /*.rendering_type     = */ Shaded);
    StringStyle comment = StringStyle(
        /*.render_color       = */ {255, 255, 255, 0},
        /*.highlight          = */ false,
        /*.highlight_color    = */ {255, 255, 255, 0},
        /*.underline          = */ false,
        /*.underline_tickness = */ 2,
        /*.underline_color    = */ {255, 255, 255, 0},
        /*.font               = */ default_font(),
        /*.rendering_type     = */ Shaded);

    // ==================================================================================
  private:
    FontType _font    = default_font();
    uint32 _font_size = 12;

    i32 glyph_width;
    i32 glyph_height;
};

} // namespace kiwi

#endif
