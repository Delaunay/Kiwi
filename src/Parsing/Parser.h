#pragma once

#include "Lexer.h"
#include "../AST/Builder.h"
#include "../Logging/Log.h"
#include "../AST/Module.h"

//#undef log_trace
//#define log_trace(...)

#define EXPECT(c, ...)\
    if (tok.type() != c){\
        log_error(__VA_ARGS__);\
    }

namespace kiwi{

    /* Parsing function:
     *          // Sanity Check
                token = peek(); check(token);
                // --------------------------;

                // Parse logic

                // -------------------------- end
                check(last_token); consume();
                return result;
     */
class Parser{
public:
    typedef typename generic::Root<Expression<LightExpression>> Root;
    typedef typename generic::DummyRoot<Expression<LightExpression>> DummyRoot;

    using RBuilder = Builder<LightExpression>;
    using FunctionArgs = std::tuple<LightExpression::ArgNames, LightExpression::ArgTypes>;
    
    Parser(AbstractBuffer& buffer):
        _lexer(buffer)
    {
        _lexer.consume();
    }

    Token nexttok(){
        _lexer.consume();
        return _lexer.peek();
    }

    void consume_token() {  _lexer.consume();       }
    Token peek_token()   {  return _lexer.peek();   }

    Dict<String, Option<Tuple<bool, uint8>>> operators = {
        { "+"     , Option<Tuple<bool, uint8>>(std::make_tuple(true , 0)) },
        { "-"     , Option<Tuple<bool, uint8>>(std::make_tuple(true , 0)) },
        { "*"     , Option<Tuple<bool, uint8>>(std::make_tuple(true , 0)) },
        { "/"     , Option<Tuple<bool, uint8>>(std::make_tuple(true , 0)) },
        { "return", Option<Tuple<bool, uint8>>(std::make_tuple(false, 0)) }
    };

    // fold_left
    Module<LightExpression> parse_declarations(int i) {
        using InsertResult = std::pair<Module<LightExpression>::iterator, bool>;
        Module<LightExpression> my_module;

        while (peek_token().type() != tok_eof) {
            Tuple<String, Root> val = parse_declaration(i + 1);
            InsertResult result = my_module.insert(
                std::make_pair(std::get<0>(val), std::get<1>(val).get()));

            if (!result.second) {
                // TODO print reference to the previous declaration so user can check
                log_warn("Declaration `", std::get<0>(val), "` already exists");
            }
        }

        return my_module;
    }

	//*/ Top level declarations
    Tuple<String, Root> parse_declaration(int i) {
        log_cdebug(i, "");
        Token tok = peek_token();

        // predict next expression from current token
        switch (tok.type()) {
        case tok_def:       return parse_function(i + 1);
        case tok_record:    return parse_record(i + 1);
        default:
            return std::make_tuple("__main__", parse_expression(i + 1));
        }

	}//*/

    Tuple<String, Root> parse_record(int i) {
        // ---------------------- Sanity Check --------------------------------
        log_cdebug(i, "");
        Token tok = peek_token(); EXPECT(tok_record, "not a record");
        // --------------------------------------------------------------------
        
        tok = nexttok(); EXPECT(tok_identifier, "expected reccord/class/struct name");
        String name = tok.identifier(); consume_token();


        // TODO
        //RBuilder::record();

        // --------------------------------------------------------------------
        EXPECT(tok_desindent, "expected desindentation got '", tok.type(), "'"); consume_token();
        return std::make_tuple(name, nullptr);
    }

    Tuple<LightExpression::ArgNames, LightExpression::ArgTypes> parse_args(int i) {
        // ---------------------- Sanity Check --------------------------------
        log_cdebug(i, "");
        Token tok = peek_token(); EXPECT('(', "expected '(' got '", tok.type(), "'");
        // --------------------------------------------------------------------
        
        LightExpression::ArgTypes arg_types;
        LightExpression::ArgNames arg_names;

        // FIXME missing )
        tok = nexttok();
        while (tok.type() != ')') {
            // handle the case where no values are present after a ','
            if (tok.type() == tok_identifier) {
                arg_names.push_back(tok.identifier());
                tok = nexttok();

                // Type annotation is optional
                if (tok.type() == ':') {
                    tok = nexttok();

                    EXPECT(tok_identifier, "expected type name");
                    log_info("Type is `", tok.identifier(), "`");
                    arg_types.push_back(RBuilder::builtin(tok.identifier()));

                    tok = nexttok();
                }

                // Has next ?
                if (tok.type() == ',')
                    tok = nexttok();
            }
        }

        // --------------------------------------------------------------------
        EXPECT(')', "expected ')' got '", tok.type(), "'"); consume_token();
        return std::make_tuple(arg_names, arg_types);
    }

    Tuple<String, Root> parse_function(int i){
        // ---------------------- Sanity Check --------------------------------
        log_cdebug(i, "");
        Token tok = peek_token(); EXPECT(tok_def, "not a function");
        // --------------------------------------------------------------------

        // >>>>>>>> Parse Prototype 
        tok = nexttok(); EXPECT(tok_identifier, "expected function name");

        String name = tok.identifier(); consume_token();
        LightExpression::ArgTypes arg_types;
        LightExpression::ArgNames arg_names;

        // >>>>>>>> Arguments
        // args = (identifier: identifier,*)
        std::tie(arg_names, arg_types) = parse_args(i);
        // <<<<<<<< Arguments

		// >>>>>>>> Return Type
        // -> return_type
        Arrow<ASTTrait>* type = nullptr;
        tok = peek_token();
		if (tok.type() == tok_arrow){
			tok = nexttok();
			EXPECT(tok_identifier, "expected type name, got '", tok.type(), "'");
			log_info("Type is `", tok.identifier(), "`");

            type = reinterpret_cast<Arrow<ASTTrait>*>(
                RBuilder::arrow(arg_types, RBuilder::builtin(tok.identifier())).take_ownership());

            consume_token();
		}
        // <<<<<<<< Return Type


        // >>>>>>>> Prototype end 
        // :\n....
        tok = peek_token();
        EXPECT(':', "':' expected, got '", int(tok.type()), "'");           tok = nexttok();
        EXPECT(tok_newline, "new line expected, got '", tok.type(), "'");    tok = nexttok();
        EXPECT(tok_indent, "indent expected, got '", int(tok.type()), "'");  consume_token();
        // <<<<<<<< Prototype end 

        Function<ASTTrait>* fun = static_cast<Function<ASTTrait>*>(RBuilder::function(name, nullptr).get());
        fun->args = arg_names;
        fun->type = type;
        fun->body = parse(i + 1);

        // --------------------------------------------------------------------
        EXPECT(tok_desindent, "expected desindentation got '", tok.type(), "'"); consume_token();
        return std::make_tuple(name, fun);
    }

    Root parse_operator(int i, String const& op, Tuple<bool, uint8> const& info) {
        Token tok = peek_token();
        //log_error(tok.type(), tok.identifier());

        // if binary
        if (std::get<0>(info)) {
            
        }

        consume_token();
        return RBuilder::unary_call(op, parse(i + 1));
    }

    Root parse(int i) {
        return parse_expression(i);
    }

    Root parse_expression(int i){
        log_cdebug(i, "");

        Root  lhs = nullptr;
        Token tok = peek_token();

        log_trace(tok);

        if (tok.type() == '('){
            consume_token();
            lhs = parse(i + 1);
            tok = peek_token();

            if (tok.type() != ')'){
                log_error("Expected closing parenthesis");
            }
        }
        else if (tok.type() == tok_float)
            lhs = parse_value(i + 1, tok.as_float());

        else if (tok.type() == tok_int)
            lhs = parse_value(i + 1, tok.as_integer());

        else if (tok.type() == tok_identifier) {
            Option<Tuple<bool, uint8>> data = operators[tok.identifier()];

            if (data.is_defined()) {
                return parse_operator(i + 1, tok.identifier(), data.get());
            }

            //log_error("'", tok.type(), "' '", tok.identifier(), "'");
            lhs = parse_identifier(i + 1, tok.identifier());
        }
            

        consume_token();        // Eat ')'/tok_int...
        tok = peek_token();

        log_trace(tok);
        tok.debug_print(std::cout) << std::endl;
        // end of expr
        if (tok.type() == tok_eof || tok.type() == ')')
            return lhs;

        // operator
        if (tok.type() == '+'){
            log_error(tok.type(), tok.identifier());
            consume_token();
            return parse_add(i + 1, lhs);
        } // FIXME 
		else {
            log_trace(tok);
            tok.debug_print(std::cout) << std::endl;
            log_error("`", int(tok.type()), "` is not a correct op (id=", tok.identifier(), ")");

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

