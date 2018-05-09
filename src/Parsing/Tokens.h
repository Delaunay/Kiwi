#pragma once

#include <unordered_map>
#include <ostream>
#include <string>
#include <vector>
#include <algorithm>

#include "../Types.h"

/*
 *  incorrect is used when the input is known to be wrong
 *  but we want to parse as much as we can anyway
 *
 *  incorrect is also the first token and eof the last
 *
 *  I could add an error policy arg
 *
 *  I think those would be nice to have
 *      Hex: 0x000A9F
 *      bin: bx010101
 */
#define KIWI_INDENT 4
#define KIWI_TOKEN \
    X(tok_identifier, -1)\
    X(tok_float,      -2)\
    X(tok_string,     -3)\
    X(tok_int,        -4)\
    X(tok_newline,    -5)\
    X(tok_indent,     -6)\
    X(tok_desindent,  -7)\
    X(tok_incorrect,  -8)\
    X(tok_eof,        -9)\
    X(tok_def,        -10)\
    X(tok_docstring,  -11)\
    X(tok_arrow,      -12)\
    X(tok_record,     -13)

#define KIWI_KEYWORDS \
    X("def"   , tok_def)\
    X("class" , tok_record)\
    X("->"    , tok_arrow)
    
namespace kiwi{

enum TokenType{
#define X(name, nb) name = nb,
    KIWI_TOKEN
#undef X
};

typedef std::unordered_map<std::string, TokenType> ReservedKeyword;
typedef std::unordered_map<int, std::string> KeywordToString;

ReservedKeyword& keywords();
KeywordToString& keyword_as_string();

uint8 tok_name_size();
std::string tok_to_string(int8 t);

class Token{
public:
    Token(TokenType t, uint32 l, uint32 c):
        _type(t), _line(l), _col(c)
    {}

    Token(int8 t, uint32 l, uint32 c):
        _type(t), _line(l), _col(c)
    {}

     int8  type()   {   return _type;   }
    uint32 line()   {   return _line;   }
    uint32 col()    {   return _col;    }

    int32 end_line()   {	return col();}
    int32 begin_line() {    return int32(col() - identifier().size()); }

    std::string& identifier() { return _identifier; }
    float64      as_float()   { return std::stod(_identifier); }
    int32        as_integer() { return std::stoi(_identifier); }

    operator bool(){
        return _type != tok_eof;
    }

private:
    int8    _type;
    uint32  _line;
    uint32  _col;

    // Data
    std::string _identifier;

public:
    // print all tokens and their info
    std::ostream& debug_print(std::ostream& out);

    // could be used for code formatting
    std::ostream& print(std::ostream& out, int indent = 0);
};

inline
Token& dummy(){
    static Token dy = Token(tok_incorrect, 0, 0);
    return dy;
}

inline
std::ostream& operator<<(std::ostream& out, class Token& tok){
    return tok.print(out);
}

// Make something that look like clang's error underlining.
// offset is used if you need to print multiple underline on a same line
inline
std::ostream& underline(std::ostream& out, Token& t, int32 offset = 0){
    int32 start = t.begin_line() - offset;
    if (start > 0){
        out << std::string(start, ' ');

        if (t.identifier().size() > 0)
            out << std::string(t.identifier().size(), '~');
        else
            out << "~";
    }

    return out;
}

}
