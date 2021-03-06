﻿#include "Tokens.h"
#include "../Format.h"

namespace kiwi
{

std::string tok_to_string(int8 t){
    switch(t){
#define X(name, nb) case nb: return std::string(#name);
    KIWI_TOKEN
#undef X
    default:
        std::string s = "' '";
        s[1] = t;
        return s;
    }
}

// this should be computed at compile time
// this is used for pretty printing
uint8 tok_name_size()
{
    std::vector<std::string> v = {
    #define X(name, nb) #name,
        KIWI_TOKEN
    #undef X
    };

    std::string::size_type max = 0;

    for (auto& i:v)
        max = std::max(i.size(), max);

    return uint8(max);
}

std::ostream& Token::debug_print(std::ostream& out){
    out << align_right(tok_to_string(_type), tok_name_size());

    out << " =>" << " [l:" << to_string_padded(_line, 4)
                  << ", c:" << to_string_padded(_col, 4) << "] " << _identifier ;
    return out;
}

// could be used for code formatting
std::ostream& Token::print(std::ostream& out, int indent){
    // Keep track of some variable for correct printing
    static int8 indent_level = 0;
    static bool emptyline = true;   // To generate indent when needed
    static bool open_parens = false;

    if (indent > 0)
        indent_level = indent;

    // because indent_level is static we need a token to reset the value
    if (type() == tok_eof){
        indent_level = 0;
        emptyline = true;
        open_parens = false;
        return out;
    }

    // Invisible Token
    if (type() == tok_indent){
        indent_level += 1;
        return out;
    }

    if (type() == tok_desindent){
        indent_level -= 1;
        return out;
    }

    if (type() == tok_newline){
        out << std::endl;
        emptyline = true;
        return out;
    }

    std::string& str = keyword_as_string()[type()];

    if (str.size() > 0){
        return out << str << " ";
    }

    // Indentation
    if (emptyline && indent_level > 0)
        out << std::string(indent_level * KIWI_INDENT, ' ');

    // Single Char
    if (type() > 0){
        if (type() == '(' || type() == '[')
            open_parens = true;
        else
            open_parens = false;

        if (type() == '=')
            out << ' ';

        emptyline = false;
        out << type();
        return out;
    }

    // Everything else
    // we printout what the lexer read in identifier (no risk of error)

    // no space if the line is empty and no space if it is just after
    // a open parens
    if (!emptyline && !open_parens)
        out << ' ';

    if (type() == tok_string)
        out << '"' << identifier() << '"' ;
    else if (type() == tok_docstring)
        out << "\"\"\"" << identifier() << "\"\"\"";
    else if (type() == tok_int || type() == tok_float)
        out << identifier() << ' ';
    else
        out << identifier();

    open_parens = false;
    emptyline = false;

    return out;
}

ReservedKeyword& keywords(){
    static ReservedKeyword _keywords = {
    #define X(str, tok) {str, tok},
        KIWI_KEYWORDS
    #undef X
    };
    return _keywords;
}

KeywordToString& keyword_as_string(){
    static KeywordToString _keywords = {
    #define X(str, tok) {tok, str},
        KIWI_KEYWORDS
    #undef X
    };
    return _keywords;
}

}
