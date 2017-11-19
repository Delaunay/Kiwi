#pragma once

#include <cctype>
#include <ostream>

#include "Buffer.h"
#include "Tokens.h"

//#define KIWI_DEBUG
#include "../Debug.h"

/*
 *  Lexer is a stream of tokens
 *
 *      TODO:   DocString support
 */

namespace kiwi {

template <typename T> class Result {};

class Lexer : public Stream<Lexer, Token> {
public:
  Lexer(AbstractBuffer &reader)
      : _reader(reader), _oindent(indent()), _cindent(indent()) {}

  // shortcuts
  const std::string &file_name() { return _reader.file_name(); }
  uint32 line() { return _reader.line(); }
  uint32 col() { return _reader.col(); }
  uint32 indent() { return _reader.indent(); }

  bool empty_line() { return _reader.empty_line(); }

  // what characters are allowed in identifiers
  bool is_identifier(char c) {
    if (std::isalnum(c) || c == '_' || c == '?' || c == '!' || c == '-')
      return true;
    return false;
  }

  Token peek() { return _token; }

  void consume() { next_token(); }

  Token next_token() {
    char c = peekc();

    // newline
    if (c == '\n') {
      _oindent = _cindent;
      _cindent = 0;
      consumec();
      return make_token(tok_newline);
    }

    if (c == EOF)
      return make_token(tok_eof);

    // indent
    if (c == ' ' && empty_line()) {
      int k = 1;
      do {
        c = nextc();

        if (c == ' ')
          k += 1;

        if (k == KIWI_INDENT) {
          consumec();
          break;
        }
      } while (c == ' ');

      _cindent = KIWI_INDENT;

      // if current indent is the same do nothing
      if (_cindent <= _oindent)
        return next_token();

      // else increase indent
      return make_token(tok_indent);
    }

    if (_cindent < _oindent) {
      _oindent -= KIWI_INDENT;
      return make_token(tok_desindent);
    }

    // remove white space
    while (c == ' ') {
      c = nextc();
    }

    // Identifiers
    if (isalpha(c)) {
      return read_identifier(c);
    }

    // Arrow
    if (c == '-') {
      c = nextc();

      if (c == '>') {
        consumec();
        return make_token(tok_arrow);
      }

      return make_token('-');
      ;
    }

    // Numbers
    if (std::isdigit(c)) {
      return read_num(c);
    }

    // strings
    if (c == '"') {
      c = nextc();
      return read_string(c);
    }

    // get next char
    c = peekc();
    consumec();
    return make_token(c);
  }

  Token read_identifier(char c) {
    std::string ident;
    ident.push_back(c);

    while (is_identifier(c = nextc())) {
      ident.push_back(c);
    }

    TokenType t = keywords()[ident];
    if (t)
      return make_token(t);
    return make_token(tok_identifier, ident);
  }

  Token read_num(char c) {
    std::string num;
    TokenType ntype = tok_int;

    while (std::isdigit(c) || c == '.') {
      num.push_back(c);

      // if it is not a digit
      if (!std::isdigit(c) && c != ' ') {
        if (c == '.') {
          if (ntype == tok_int)
            ntype = tok_float;
          else
            ntype = tok_incorrect; // 12.12.12 is an incorrect token
        } else                     // 0x for hex is okay
          // bx is not  since it would be a correct identifier
          // 0b ?
          ntype = tok_incorrect; // 1abc is an incorrect token
      }

      c = nextc();
    }

    return make_token(ntype, num);
  }

  Token read_string(char c) {
    std::string str;
    int k = 1;

    // empty
    if (c == '"') {
      c = nextc();

      if (c != '"')
        return make_token(tok_string, "");

      c = nextc();
      k = 3;
    }

    int i = 0;
    while (i < k) {
      // dont add closing quotes
      if (c == '"') {
        i += 1;
      } else {
        // " were not closing quotes we need to add them
        if (i != 0) {
          for (int n = 0; n < i; ++n)
            str.push_back('"');
        }
        str.push_back(c);
      }
      c = nextc();
    }

    Token tok = dummy();
    if (k == 1)
      tok = make_token(tok_string, str);
    else
      tok = make_token(tok_docstring, str);

    consumec();
    return tok;
  }

  Token make_token(int8 t) {
    _token = Token(t, line(), col());
    return _token;
  }

  Token make_token(int8 t, const std::string &identifier) {
    _token = Token(t, line(), col());
    _token.identifier() = identifier;
    return _token;
  }

private:
  AbstractBuffer &_reader;
  Token _token{dummy()};
  uint32 _oindent;
  uint32 _cindent;

  // debug
public:
  // print tokens with their info
  std::ostream &debug_print(std::ostream &out);

  // print out tokens as they were inputed
  std::ostream &print(std::ostream &out);

  // extract a token stream into a token vector
  std::vector<Token> extract_token() {
    std::vector<Token> v;

    Token t = next_token();
    do {
      v.push_back(t);
    } while ((t = next_token()));

    v.push_back(t); // push eof token
    return v;
  }

private:
  // helpers
  void consumec() { return _reader.consume(); }
  char peekc() { return _reader.peek(); }
  Token &token() { return _token; }
  char nextc() {
    _reader.consume();
    return _reader.peek();
  }
};
}
