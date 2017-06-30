#pragma once

#include "Lexer.h"
#include "../AST/Builder.h"

//#define KIWI_DEBUG
#include "../Debug.h"
#include "../Logging/Log.h"

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
    Root parse_function(int i){
        log_cdebug(i);

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

        Root body = parse(i + 1);
        fun->body = body;

        return fun;
    }

    Root parse(int i){
        log_cdebug(i);

        Root  lhs = nullptr;
        Token tok = _lexer.peek();

        dumptok(tok);

        if (tok.type() == '('){
            _lexer.consume();
            lhs = parse(i + 1);

            tok = _lexer.peek();

            if (tok.type() != ')'){
                printd("Expected closing parenthesis");
            }
        }
        else if (tok.type() == tok_float)
            lhs = parse_value(i + 1, tok.as_float());

        else if (tok.type() == tok_int)
            lhs = parse_value(i + 1, tok.as_integer());

        else if (tok.type() == tok_identifier)
            lhs = parse_identifier(i + 1, tok.identifier());

        _lexer.consume();       // Eat ')'/tok_int...
        tok = _lexer.peek();

        dumptok(tok);

        // end of expr
        if (tok.type() == tok_eof || tok.type() == ')')
            return lhs;

        // operator
        if (tok.type() == '+'){
            _lexer.consume();
            return parse_add(i + 1, lhs);
        } else {
            dumptok(tok);
            printd("Is not a correct op");

            if (lhs.get() == nullptr){
                printd("nullptr");
            }
            print_expr(std::cout, lhs);
            std::cout << std::endl;
        }
        return lhs;
    }

    Root parse_add(int i, Root lhs){
        log_cdebug(i);
        Root rhs = parse(i + 1);

        if (rhs.get() == nullptr)
            return lhs;

        return RBuilder::add(lhs, rhs);
    }

    Root parse_value(int i, double val){
        log_cdebug(i);
        return RBuilder::value(val);
    }

    Root parse_identifier(int i, std::string name){
        log_cdebug(i);
        return RBuilder::placeholder(name);
    }

private:
    Lexer _lexer;
    // Context _ctx;
};

}

