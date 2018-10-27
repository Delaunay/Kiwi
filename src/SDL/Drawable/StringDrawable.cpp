#include "StringDrawable.h"
#include "../StyleManager.h"

namespace kiwi {

StringDrawable::StringDrawable(const std::string &str, const StringStyle &style) :
    _string(str), _style(style) {
    log_info("Making StringDrawable: ", _string);
    string_to_surface(true);
}

void StringDrawable::string_to_surface(bool) {
    _surface = SurfacePtr{render_utf8()};
    int w, h;

    TTF_SizeUTF8(_style.font, &_string[0], &w, &h);

    _size.x = f32(std::max(w, 10));
    _size.y = f32(std::max(h, 10));
}

void StringDrawable::underline(SDL_Renderer *renderer, int tickness, SDL_Color color) const {
    log_trace();
    drawHorizontalLine(size().y - tickness, renderer, tickness, color);
}

void StringDrawable::strike(SDL_Renderer *renderer, int tickness, SDL_Color color) const {
    log_trace();
    drawHorizontalLine(size().y / 2.0 - tickness / 2, renderer, tickness, color);
}

void StringDrawable::drawHorizontalLine(int y, SDL_Renderer *renderer, int tickness,
                                        SDL_Color color) const {
    log_trace();
    SDL_Color old;
    SDL_GetRenderDrawColor(renderer, &old.r, &old.g, &old.b, &old.a);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect line = {0, y, size().x, tickness};
    SDL_RenderFillRect(renderer, &line);
    SDL_SetRenderDrawColor(renderer, old.r, old.g, old.b, old.a);
}

Point StringDrawable::off_screen_draw(SDL_Renderer *renderer, Point position) {
    log_trace();
    if(!_texture) {
        // Surface To Texture
        string_to_surface();

        // log_info("Generating Texture (str: ", _string, ")");
        // Create Offscreen Rendering Target
        _texture    = TexturePtr{CHECK(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                                      SDL_TEXTUREACCESS_TARGET, _size.x, _size.y))};
        auto target = SDL_GetRenderTarget(renderer);
        CHECK(SDL_SetRenderTarget(renderer, _texture.get()));
        Color c = StyleManager::style_manager().background_color;
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        SDL_RenderClear(renderer);

        if(_string.size() > 0) {
            auto text = TexturePtr{CHECK(SDL_CreateTextureFromSurface(renderer, _surface.get()))};

            // Render Text
            SDL_Rect box;
            box.x = i32(0);
            box.y = i32(0);
            box.w = i32(_size.x);
            box.h = i32(_size.y);
            CHECK(SDL_RenderCopy(renderer, text.get(), nullptr, &box));
        }

        // Styling
        // strike(renderer, 2, { 250, 150, 250 });

        if(_style.underline)
            underline(renderer, _style.underline_tickness, _style.underline_color);

        // Draw Bounding box
        _count += 1;
        // int r = _count % 2 == 0 ? 250 : 150;

        if(BackEndConfig::config().show_string_bound_box) {
            Color c = BackEndConfig::config().string_bound_box_color;
            SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
            SDL_RenderDrawRect(renderer, nullptr);
        }

        CHECK(SDL_SetRenderTarget(renderer, target));
    }
    return {position.x + _size.x, position.y};
}

Point StringDrawable::draw(SDL_Renderer *renderer, Point position) {
    log_trace();
    // Rendering into its own Sufrace so position is 0, 0
    off_screen_draw(renderer, {0, 0});

    SDL_Rect box;
    box.x = i32(position.x);
    box.y = i32(position.y);
    box.w = i32(_size.x);
    box.h = i32(_size.y);

    CHECK(SDL_RenderCopy(renderer, _texture.get(), nullptr, &box));
    return {position.x + _size.x, position.y};
}

// Blended, _string, _font, _style.render_color
SDL_Surface *StringDrawable::render_utf8() {
    log_trace();
    RenderingType override = _style.rendering_type;
    if(_style.highlight)
        override = Shaded;

    switch(override) {
    case RenderingType::Solid:
        return CHECK(TTF_RenderUTF8_Solid(_style.font, _string.c_str(), _style.render_color));
    case RenderingType::Blended:
        return CHECK(TTF_RenderUTF8_Blended(_style.font, _string.c_str(), _style.render_color));
    case RenderingType::Shaded: {
        SDL_Color highlight_override = StyleManager::style_manager().background_color;
        if(_style.highlight)
            highlight_override = _style.highlight_color;
        return CHECK(TTF_RenderUTF8_Shaded(_style.font, _string.c_str(), _style.render_color,
                                           highlight_override));
    }
    }
    return CHECK(TTF_RenderUTF8_Solid(_style.font, _string.c_str(), _style.render_color));
}

void StringDrawable::set_string(String const &str) {
    log_trace();
    string_to_surface();
    _string = str;
    _texture.release();
}

// from http://www.zedwood.com/article/cpp-utf8-strlen-function
int StringDrawable::utf8_strlen(String const &str) {
    log_trace();

    int c, i, ix, q;
    for(q = 0, i = 0, ix = str.length(); i < ix; i++, q++) {
        c = (unsigned char)str[i];
        if(c >= 0 && c <= 127)
            i += 0;
        else if((c & 0xE0) == 0xC0)
            i += 1;
        else if((c & 0xF0) == 0xE0)
            i += 2;
        else if((c & 0xF8) == 0xF0)
            i += 3;
        // else if (($c & 0xFC) == 0xF8) i+=4; // 111110bb //byte 5, unnecessary in 4 byte UTF-8
        // else if (($c & 0xFE) == 0xFC) i+=5; // 1111110b //byte 6, unnecessary in 4 byte UTF-8
        else
            return 0; // invalid utf8
    }
    return q;
}

i64 StringDrawable::get_char_byte_size(const String &str, u64 byte_pos) {
    log_trace();
    i32 c = i32(uchar(str[byte_pos]));

    if(c >= 0 && c <= 127)
        return 1;
    else if((c & 0xE0) == 0xC0)
        return 2;
    else if((c & 0xF0) == 0xE0)
        return 3;
    else if((c & 0xF8) == 0xF0)
        return 4;
    else
        return 0; // invalid utf8
}

i64 StringDrawable::get_byte_loc(const String &str, i64 char_pos) {
    log_trace();
    i64 bsize = 0;
    i64 count = 0;
    for(i64 i = 0, n = i64(str.length()); i < n;) {
        if(count == char_pos)
            return i64(i);

        bsize = get_char_byte_size(str, i);
        count += 1;
        i += bsize;
    }
    return i64(-1);
}

i32 StringDrawable::glyph_width() const {
    log_trace();
    return StyleManager::style_manager().width();
}

i32 StringDrawable::glyph_height() const {
    log_trace();
    return StyleManager::style_manager().height();
}
} // namespace kiwi
