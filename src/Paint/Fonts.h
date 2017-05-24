#pragma once

#include <string>
#include <SFML/Graphics/Font.hpp>

#define HOME "../../fonts/"

namespace kiwi{

    /*
    const std::string& font_folder(){
        static std::string font_repo = "C:/Windows/Fonts/";
        return font_repo;
    }

    sf::Font lookupFont(const std::string& font_name){
        sf::Font font;
        return font.loadFromFile(font_repo + font_name);
    } */

    inline
    sf::Font load_font(const std::string& str){
        sf::Font font;
        font.loadFromFile(str);
        return font;
    }

    inline
    const sf::Font& default_font(){
        static sf::Font font = load_font(HOME "consola.ttf");
        return font;
    }
}
