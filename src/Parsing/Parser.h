#pragma once

#include "Lexer.h"
#include "../AST/Builder.h"
#include "../Logging/Log.h"

#undef log_trace
#define log_trace(...)

#define EXPECT(c, ...)\
    if (tok.type() != c){\
        log_error(__VA_ARGS__);\
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


	/*/ Top level declarations
	Root parse_declaration() {
	

	}*/

    // not tested
    Root parse_function(int i){
        log_cdebug(i, "");

        Root  lhs = nullptr;
        Token tok = _lexer.peek();

        EXPECT(tok_def, "not a function");
        tok = nexttok();

        EXPECT(tok_identifier, "expected function name");
        std::string name = tok.identifier();

        Tree::Function* fun = static_cast<Tree::Function*>(
                    RBuilder::function(name, nullptr).get());

        tok = nexttok();
		
        EXPECT('(', "expected '(' got '", tok.type(), "'");
        tok = nexttok();

		Expression::Args args;

		// FIXME missing )
        while(tok.type() != ')'){

            //EXPECT(tok_identifier, "expected arg name");

			// handle the case where no values are present after a ','
            if (tok.type() == tok_identifier){
				fun->args.push_back(tok.identifier());
				tok = nexttok();

				// Type annotation is optional
				if (tok.type() == ':') {
					tok = nexttok();

					EXPECT(tok_identifier, "expected type name");
					log_info("Type is `", tok.identifier(), "`");
					args.push_back(new Tree::Type(tok.identifier()));

					tok = nexttok();
				}

				// Has next ?
				if (tok.type() == ',')
					tok = nexttok();
			}
        }
		EXPECT(')', "expected ')' got, '", tok.type(), "'");
		tok = nexttok();

		//std::cout << "========================================================\n";
		//std::cout << tok.type() << std::endl;

		// return type annotation
		if (tok.type() == tok_arrow){
			tok = nexttok();
			EXPECT(tok_identifier, "expected type name, got '", tok.type(), "'");
			log_info("Type is `", tok.identifier(), "`");

			fun->type = reinterpret_cast<Tree::Arrow*>(RBuilder::arrow(args, RBuilder::builtin(tok.identifier())).take_ownership());
			tok = nexttok();

			//std::cout << int(fun->type) << std::endl;

			std::cout << "========================================" << std::endl;
			std::stringstream ss;
			print_expr(ss, fun->type);
			std::cout << ss.str() << std::endl << "========================================";
		}

        EXPECT(':', "':' expected, got '", int(tok.type()), "'");
        tok = nexttok();
        EXPECT(tok_newline, "new line expected, got'", tok.type(), "'");

        tok = nexttok();
		EXPECT(tok_indent, "indent expected, got'", int(tok.type()), "'");
		tok = nexttok();

        Root body = parse(i + 1);
        fun->body = body;

        return fun;
    }

    Root parse(int i){
        log_cdebug(i, "");

        Root  lhs = nullptr;
        Token tok = _lexer.peek();

        log_trace(tok);

        if (tok.type() == '('){
            _lexer.consume();
            lhs = parse(i + 1);

            tok = _lexer.peek();

            if (tok.type() != ')'){
                log_error("Expected closing parenthesis");
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

        log_trace(tok);

        // end of expr
        if (tok.type() == tok_eof || tok.type() == ')')
            return lhs;

        // operator
        if (tok.type() == '+'){
            _lexer.consume();
            return parse_add(i + 1, lhs);
        } // FIXME 
		else {
            log_trace(tok);
            log_error("`", int(tok.type()), "` is not a correct op");

            if (lhs.get() == nullptr){
                log_error("nullptr");
            } else {
				//print_expr(std::cout, lhs);
				//std::cout << std::endl;
			}
        }

        return lhs;
    }

    Root parse_add(int i, Root lhs){
        log_cdebug(i, "");
        Root rhs = parse(i + 1);

        if (rhs.get() == nullptr)
            return lhs;

        return RBuilder::add(lhs, rhs);
    }

    Root parse_value(int i, double val){
        log_cdebug(i, "");
        return RBuilder::value(val);
    }

    Root parse_identifier(int i, std::string name){
        log_cdebug(i, "");
        return RBuilder::placeholder(name);
    }

private:
    Lexer _lexer;
    // Context _ctx;
};

}

