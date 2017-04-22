#pragma once

#include "Lexer.h"
#include "../AST/Builder.h"

//#define printd(x) std::cout << x << std::endl
//#define dumptok(tol) tok.debug_print(std::cout) << std::endl;
#define printd(x)
#define dumptok(tol)

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

        if (tok.type() == tok_float)
            lhs = parse_value(tok.as_float());

        else if (tok.type() == tok_int)
            lhs = parse_value(tok.as_integer());

        _lexer.consume();
        tok = _lexer.peek();

        dumptok(tok);

        if (tok.type() == '+'){
            _lexer.consume();
            return parse_add(lhs);
        }
        return lhs;
    }

    Root parse_add(Root lhs){
        printd("parse_add");
        Root rhs = parse();
        return Builder<>::add(lhs, rhs);
    }

    Root parse_value(double val){
        printd("parse_val");
        return Builder<>::value(val);
    }


private:
    Lexer _lexer;
};

}
