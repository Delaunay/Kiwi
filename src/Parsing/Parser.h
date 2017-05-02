#pragma once

#include "Lexer.h"
#include "../AST/Builder.h"

//#define KIWI_DEBUG
#include "../Debug.h"

namespace kiwi{

class Parser{
public:
    Parser(AbstractBuffer& buffer):
        _lexer(buffer)
    {
        _lexer.consume();
    }

    Root parse(){
        printd("parse_parse");

        Root  lhs = nullptr;
        Token tok = _lexer.peek();

        dumptok(tok);

        if (tok.type() == '('){
            _lexer.consume();
            lhs = parse();

            tok = _lexer.peek();

            if (tok.type() != ')'){
                printd("Expected closing parenthesis");
            }
        }
        else if (tok.type() == tok_float)
            lhs = parse_value(tok.as_float());

        else if (tok.type() == tok_int)
            lhs = parse_value(tok.as_integer());

        else if (tok.type() == tok_identifier)
            lhs = parse_identifier(tok.identifier());

        _lexer.consume();       // Eat ')'/tok_int...
        tok = _lexer.peek();

        dumptok(tok);

        // end of expr
        if (tok.type() == tok_eof || tok.type() == ')')
            return lhs;

        // operator
        if (tok.type() == '+'){
            _lexer.consume();
            return parse_add(lhs);
        } else {
            dumptok(tok);
            printd("Is not a correct op");

            if (lhs.get() == nullptr){
                printd("nullptr");
            }
            print(std::cout, lhs);
            std::cout << std::endl;
        }
        return lhs;
    }

    Root parse_add(Root lhs){
        printd("parse_add");
        Root rhs = parse();

        if (rhs.get() == nullptr)
            return lhs;

        return Builder<>::add(lhs, rhs);
    }

    Root parse_value(double val){
        printd("parse_val");
        return Builder<>::value(val);
    }

    Root parse_identifier(std::string name){
        printd("parse_identifier");
        return Builder<>::placeholder(name);
    }

private:
    Lexer _lexer;
    // Context _ctx;
};

}

