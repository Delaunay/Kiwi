#pragma once

#include "RenderNode.h"

namespace kiwi{

struct StyleManager{
private:
    StyleManager(){
        log_info("Style Manager was created");
    }

public:
    typedef SDLString StringType;
    typedef SDL_Font* FontType;

    static StyleManager& style_manager(){
        static StyleManager style;
        return style;
    }

    const StringType& get(char c) const{
        switch (c) {
        case '(': return fun_open;
        case ')': return fun_close;
        case ':': return fun_col;
        case ' ': return _space;
        case ',': return fun_com;
        default:
            log_error("Requesting an non-predef");
            return _space;
        }
    }

    const StringType& def()  const { return fun_def;}
    const StringType& arrow()const { return _arrow;}
    const StringType& space()const { return _space;}

    float width()const { return glyph_width;}
    float height()const { return glyph_height;}

    float width(const StringType& txt) const{ return txt.width();}
    float height(const StringType&) const{ return glyph_height;}

    Point size(const StringType& txt) const{ return Point(width(txt), height(txt));}

    const StringType& open_parenthesis() const{ return fun_open;}
    const StringType& close_parenthesis() const{ return fun_close;}

    template<typename T>
    StringType text(const T& str){ return RenderTrait::make_string(str); }

    template<typename T>
    StringType keyword(const T& str){ return RenderTrait::make_keyword(str); }

    const FontType& font()   const {  return _font;       }
    unsigned int font_size() const {  return _font_size;   }

private:
    FontType _font = SDLString::default_font();
    unsigned int _font_size = 12;

    StringType fun_def   = keyword("def ");
    StringType fun_open  = text("(");
    StringType fun_close = text(")");
    StringType fun_col   = text(": ");
    StringType fun_com   = text(", ");
    StringType _space    = text(" ");
    StringType _arrow    = text(u8" \u2192 ");

    float glyph_width = _space.width();   // space.getCharacterSize();
    float glyph_height = _space.height(); // space.getGlobalBounds().height;
};

}
