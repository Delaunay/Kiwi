#ifndef KIWI_STRINGDRAWABLE_HEADER
#define KIWI_STRINGDRAWABLE_HEADER

#define SDL_MAIN_HANDLED
#include <SDL.h>

#ifdef __linux__
#   include <SDL2/SDL_ttf.h>
#else
#   include <SDL_ttf.h>
#endif

#include "../SDLHelper.h"
#include "../Point.h"
#include "Drawable.h"

#undef log_trace
#define log_trace(...)


#include <algorithm>


namespace kiwi {
    typedef TTF_Font SDL_Font;

    enum RenderingType {
        Solid,
        Blended,
        Shaded
    };

// Create an Offscreen Rendering Target (Texture)
// Render to Surface if needed
// Draw to Rendering Target (Origin: (0, 0))
// Copy Rendering Target to Screen at the given position

class StringDrawable : public Drawable {
public:
    StringDrawable(const std::string& str, SDL_Color color = default_color(), SDL_Font* font = default_font()):
        _string(str), _font(font), _color(color)
    {
        log_info("Making StringDrawable: ", str);
        string_to_surface(true);
    }

    void string_to_surface(bool init = false) {
        _surface = SurfacePtr{ render_utf8(Blended, _string,  _font, _color) };
        int w, h;

        TTF_SizeUTF8(_font, &_string[0], &w, &h);

        _size.x = f32(std::max(w, 10));
        _size.y = f32(std::max(h, 10));
    }

    void reset_cache() {
        log_trace();
        _texture = nullptr;
    }

    void underline(SDL_Renderer *renderer, int tickness, SDL_Color color) const {
        log_trace();
        drawHorizontalLine(size().y - tickness, renderer, tickness, color);
    }

    void strike(SDL_Renderer *renderer, int tickness, SDL_Color color) const {
        log_trace();
        drawHorizontalLine(size().y / 2.0 - tickness / 2, renderer, tickness, color);
    }

    void drawHorizontalLine(int y, SDL_Renderer *renderer, int tickness, SDL_Color color) const {
        log_trace();
        SDL_Color old;
        SDL_GetRenderDrawColor(renderer, &old.r, &old.g, &old.b, &old.a);
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_Rect line = { 0, y, size().x, tickness };
        SDL_RenderFillRect(renderer, &line);
        SDL_SetRenderDrawColor(renderer, old.r, old.g, old.b, old.a);
    }

    int mutable _count = 0;
    Point off_screen_draw(SDL_Renderer *renderer, Point position) {
        log_trace();
        if (!_texture) {
            // Surface To Texture
            string_to_surface();

            //log_info("Generating Texture (str: ", _string, ")");
            // Create Offscreen Rendering Target
            _texture = TexturePtr{ CHECK(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, _size.x, _size.y)) };
            auto target = SDL_GetRenderTarget(renderer);
            CHECK(SDL_SetRenderTarget(renderer, _texture.get()));
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            if (_string.size() > 0){
                auto text = TexturePtr{ CHECK(SDL_CreateTextureFromSurface(renderer, _surface.get())) };

                // Render Text
                SDL_Rect box;
                box.x = i32(0);
                box.y = i32(0);
                box.w = i32(_size.x);
                box.h = i32(_size.y);
                CHECK(SDL_RenderCopy(renderer, text.get(), nullptr, &box));
            }

            // Styling
            //strike(renderer, 2, { 250, 150, 250 });
            //underline(renderer, 2, { 150, 250, 250 });

            // Draw Bounding box
            _count += 1;
            //int r = _count % 2 == 0 ? 250 : 150;

            SDL_SetRenderDrawColor(renderer, 250, 250, 150, 0);
            SDL_RenderDrawRect(renderer, nullptr);

            CHECK(SDL_SetRenderTarget(renderer, target));
        }
        return { position.x + _size.x, position.y};
    }
    

    Point draw(SDL_Renderer *renderer, Point position) override {
        log_trace();
        // Rendering into its own Sufrace so position is 0, 0
        off_screen_draw(renderer, { 0, 0 });
        
        SDL_Rect box;
        box.x = i32(position.x);
        box.y = i32(position.y);
        box.w = i32(_size.x);
        box.h = i32(_size.y);
        
        CHECK(SDL_RenderCopy(renderer, _texture.get(), nullptr, &box));
        return { position.x + _size.x, position.y};
    }

    Point size() const override { return _size; }

    SDL_Surface* render_utf8(RenderingType kind, const String& txt, SDL_Font* font, SDL_Color color, SDL_Color color_bg = {0, 50, 0}) {
        log_trace();
        CHECK(font);
        switch (kind) {
        case RenderingType::Solid:   return CHECK(TTF_RenderUTF8_Solid(font, txt.c_str(), color));
        case RenderingType::Blended: return CHECK(TTF_RenderUTF8_Blended(font, txt.c_str(), color));
        case RenderingType::Shaded:  return CHECK(TTF_RenderUTF8_Shaded(font, txt.c_str(), color, color_bg));
        }
        return CHECK(TTF_RenderUTF8_Solid(font, txt.c_str(), color));
    }

    static SDL_Font* default_font() {
        log_trace();
        static SDL_Font* font = CHECK(TTF_OpenFont(HOME "DejaVuSansMonoPowerline.ttf", 24));
        return font;
    }

    static SDL_Color default_color() {
        log_trace();
        return SDL_Color{ 255, 255, 255 };
    }

    void set_string(String const& str) {
        log_trace();
        string_to_surface();
        _string = str;
        _texture.release();
    }

    const char* c_string() const {
        return _string.c_str();
    }

    const String& string() const {
        log_trace();
        return _string;
    }

    int string_size() const {
        log_trace();
        return _string.size();
    }

    SDL_Texture* texture() const {
        log_trace();
        return _texture.get();
    }

    int length() const { return utf8_strlen(string()); }

    // from http://www.zedwood.com/article/cpp-utf8-strlen-function
    static int utf8_strlen(String const& str)
    {
        log_trace();

        int c, i, ix, q;
        for (q = 0, i = 0, ix = str.length(); i < ix; i++, q++)
        {
            c = (unsigned char)str[i];
            if (c >= 0 && c <= 127) i += 0;
            else if ((c & 0xE0) == 0xC0) i += 1;
            else if ((c & 0xF0) == 0xE0) i += 2;
            else if ((c & 0xF8) == 0xF0) i += 3; 
            //else if (($c & 0xFC) == 0xF8) i+=4; // 111110bb //byte 5, unnecessary in 4 byte UTF-8
            //else if (($c & 0xFE) == 0xFC) i+=5; // 1111110b //byte 6, unnecessary in 4 byte UTF-8
            else return 0;//invalid utf8
        }
        return q;
    }

    static int get_char_byte_size(const String& str, int byte_pos) {
        log_trace();
        int c = (unsigned char)str[byte_pos];

        if (c >= 0 && c <= 127)       return 1;
        else if ((c & 0xE0) == 0xC0)  return 2;
        else if ((c & 0xF0) == 0xE0)  return 3;
        else if ((c & 0xF8) == 0xF0)  return 4;
        else return 0;//invalid utf8
    }

    static int get_byte_loc(const String& str, int char_pos) {
        log_trace();
        int bsize = 0, count = 0;
        for (int i = 0; i < str.length();) {
            if (count == char_pos)
                return i;

            bsize = get_char_byte_size(str, i);
            count += 1;
            i += bsize;
        }
        return -1;
    }

    i32 glyph_width() const;
    i32 glyph_height() const;

    // FIXME: find where this is required
    operator const String& () const {
        return _string;
    }

    String const drawable_name() override {
        return String("StringDrawable(\"") + _string + "\")";
    }

private:
    Point       _size;
    String      _string;
    SurfacePtr  _surface;
    mutable TexturePtr _texture;

    SDL_Color _color;
    SDL_Font* _font;
};
}

#endif