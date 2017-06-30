#pragma once

#include "Fonts.h"
#include <SFML/Graphics.hpp>

namespace kiwi{

struct StyleManager{
    StyleManager(){
        log_info("Style Manager was created");
    }

    const sf::Text& get(char c){
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

    const sf::Text& def(){ return fun_def;}
    const sf::Text& arrow(){ return _arrow;}
    const sf::Text& space(){ return _space;}

    float width(){ return glyph_width;}
    float height(){ return glyph_height;}

    float width(const sf::Text& txt){ return glyph_width * txt.getString().getSize();}
    float height(const sf::Text&){ return glyph_height;}

    sf::Vector2f size(const sf::Text& txt){ return {width(txt), height(txt)};}

    const sf::Text& open_parenthesis(){ return fun_open;}
    const sf::Text& close_parenthesis(){ return fun_close;}

    const sf::Text text(const std::string& str){
        return sf::Text(str, font(), font_size());
    }

    const sf::Text text(const std::wstring& str){
        return sf::Text(str,font(), font_size());
    }

    const sf::Font& font()   {  return _font;       }
    unsigned int font_size() {  return _font_size;   }

private:
    sf::Font _font = default_font();
    unsigned int _font_size = default_font_size();

    sf::Text fun_def   = text("def ");
    sf::Text fun_open  = text("(");
    sf::Text fun_close = text(")");
    sf::Text fun_col   = text(":");
    sf::Text fun_com   = text(", ");
    sf::Text _space    = text(" ");
    sf::Text _arrow    = text(L" \u2192 ");

    float glyph_width = _space.getGlobalBounds().width;   // space.getCharacterSize();
    float glyph_height = _font.getLineSpacing(_font_size); // space.getGlobalBounds().height;
};

}
