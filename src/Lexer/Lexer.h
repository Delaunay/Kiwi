#pragma once

#include <ostream>

#include "Buffer.h"
#include "Tokens.h"
#include "../Types.h"

#include <cctype>

#include <iostream>

/*
 *  Lexer is a stream of tokens
 *
 *      TODO:   DocString support
 */

namespace kiwi{

class Lexer
{
public:

    Lexer(AbstractBuffer& reader):
        _reader(reader), _cindent(indent()), _oindent(indent())
    {}

    // shortcuts
    const std::string& file_name()   {   return _reader.file_name();  }
    uint32 line()      {    return _reader.line();      }
    uint32 col()       {    return _reader.col();       }
    uint32 indent()    {    return _reader.indent();    }
    void   consume()   {    return _reader.consume();   }
    char   peek()      {    return _reader.peek();      }
    bool   empty_line(){    return _reader.empty_line();}
    Token& token()     {    return _token;              }
    char   nextc(){
        _reader.consume();
        return _reader.peek();
    }

    // what characters are allowed in identifiers
    bool is_identifier(char c){
        if (std::isalnum(c) || c == '_' || c == '?' || c == '!' || c == '-')
            return true;
        return false;
    }


    Token next_token(){
        char c = peek();

        // newline
        if (c == '\n'){
            _oindent = _cindent;
            _cindent = 0;
            consume();
            return make_token(tok_newline);
        }

        if (c == EOF)
            return make_token(tok_eof);

        // indent
        if (c == ' ' && empty_line()){
            int k = 1;
            do{
                c = nextc();
                k++;

                if (k == LYTHON_INDENT && c == ' '){
                    consume();
                    break;
                }
            } while(c == ' ');

            _cindent += LYTHON_INDENT;

            // if current indent is the same do nothing
            if (_cindent <= _oindent)
                return next_token();

            // else increase indent
            return make_token(tok_indent);
        }

        if (_cindent < _oindent){
            _oindent -= LYTHON_INDENT;
            return make_token(tok_desindent);
        }

        // remove white space
        while (c == ' '){
            c = nextc();
        }

        // Identifiers
        if (isalpha(c)){
            std::string ident;
            ident.push_back(c);

            while(is_identifier(c = nextc())){
                ident.push_back(c);
            }

            TokenType t = keywords()[ident];
            if (t)  return make_token(t);
            return make_token(tok_identifier, ident);
        }

        // Arrow
        if (c == '-'){
            c = nextc();

            if (c == '>'){
                consume();

                return make_token(tok_arrow);
            }

            return make_token('-');;
        }

        // Numbers
        if (std::isdigit(c)){
            std::string num;
            TokenType ntype = tok_int;

            while(c != ' ' && c != EOF && c != '\n')
            {
                num.push_back(c);

                // if it is not a digit
                if (!std::isdigit(c) && c != ' '){
                    if (c == '.'){
                        if (ntype == tok_int)
                            ntype = tok_float;
                        else
                            ntype = tok_incorrect;    // 12.12.12 is an incorrect token
                    }
                    else  // 0x for hex is okay
                          // bx is not  since it would be a correct identifier
                          // 0b ?
                        ntype = tok_incorrect;        // 1abc is an incorrect token
                }

                c = nextc();
            }

            return make_token(ntype, num);
        }

        // strings
        if (c == '"'){
            std::string str;

            while((c = nextc()) != '"'){
                str.push_back(c);
            }

            /* Check for doc string
            if (c == '"'){
                c = nextc();
                if (c == '"'){
                    int k = 0;
                    c = nextc();
                    while(k != 3){
                        if (c == '"'){
                            k += 1;
                        }
                        else {
                            k = 0;
                            str.push_back(c);
                            c = nextc();
                        }
                    }
                    c = nextc();
                    return make_token(tok_docstring, str);
                }
            }*/

            consume();
            return make_token(tok_string, str);
        }

        // get next char
        c = peek();
        consume();
        return make_token(c);
    }

    Token make_token(int8 t){
        _token = Token(t, line(), col());
        return _token;
    }

    Token make_token(int8 t, const std::string& identifier){
        _token = Token(t, line(), col());
        _token.identifier() = identifier;
        return _token;
    }

private:
    AbstractBuffer& _reader;
    Token           _token{dummy()};
    uint32          _oindent;
    uint32          _cindent;

// debug
public:

    // print tokens with their info
    std::ostream& debug_print(std::ostream& out);

    // print out tokens as they were inputed
    std::ostream& print(std::ostream& out);

    // extract a token stream into a token vector
    std::vector<Token> extract_token(){
        std::vector<Token> v;

        Token t = next_token();
        do{
            v.push_back(t);
        }while(t = next_token());

        v.push_back(t); // push eof token
        return v;
    }
};



}