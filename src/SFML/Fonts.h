#pragma once

#include <SFML/Graphics/Font.hpp>
#include "../Logging/Log.h"

#define HOME "../../fonts/"

#include <cassert>

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
        bool ret = font.loadFromFile(str);
        assert(ret && "Unable to load font");

        log_info(" Font: " + std::to_string(ret));

        return font;
    }

    inline
    const sf::Font& default_font(){
        static sf::Font font = load_font(HOME "DejaVuSansMono.ttf");
        return font;
    }

    inline
    unsigned int default_font_size(){
        return 18;
    }
}
