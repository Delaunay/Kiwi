#pragma once

#include "Lexer.h"
#include "../AST/Builder.h"

//#define KIWI_DEBUG
#include "../Debug.h"


#define EXPECT(c, message)\
    if (tok.type() != c){\
        printd(message);\
    }

namespace kiwi{

class Parser{
public:
    typedef typename generic::Root<Expression> Root;
    typedef typename generic::DummyRoot<Expression> DummyRoot;
    typedef Builder<Root, LightAST> RBuilder;
    typedef LightAST Tree;

    Parser(AbstractBuffer& buffer):
        _lexer(buffer)
    {
        _lexer.consume();
    }

    Token nexttok(){
        _lexer.consume();
        return _lexer.peek();
    }

    // not tested
    Root parse_function(){
        printd("parse_function");

        Root  lhs = nullptr;
        Token tok = _lexer.peek();

        EXPECT(tok_def, "not a function");

        tok = nexttok();
        EXPECT(tok_identifier, "expected function name");

        std::string name = tok.identifier();
        Tree::Function* fun = static_cast<Tree::Function*>(
                    RBuilder::function(name, nullptr).get());

        tok = nexttok();
        EXPECT('(', "'(' expected");
        tok = nexttok();

        while(tok.type() != ')'){
            EXPECT(tok_identifier, "expected arg name");
            fun->args.push_back(tok.identifier());
            tok = nexttok();

            if (tok.type() == ',')
                tok = nexttok();
        }

        EXPECT(':', "':' expected");
        tok = nexttok();

        EXPECT(tok_newline, "new line expected");
        tok = nexttok();

        Root body = parse();
        fun->body = body;

        return fun;
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

        return RBuilder::add(lhs, rhs);
    }

    Root parse_value(double val){
        printd("parse_val");
        return RBuilder::value(val);
    }

    Root parse_identifier(std::string name){
        printd("parse_identifier");
        return RBuilder::placeholder(name);
    }

private:
    Lexer _lexer;
    // Context _ctx;
};

}

