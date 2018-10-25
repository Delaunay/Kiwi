#pragma once

#include "../AST/Builder.h"
#include "../AST/Module.h"
#include "../Logging/Log.h"
#include "Lexer.h"

//#undef log_trace
//#define log_trace(...)

#define EXPECT(c, ...)                                                                             \
    if(tok.type() != c) {                                                                          \
        log_error(__VA_ARGS__);                                                                    \
        tok.debug_print(std::cout) << std::endl;                                                   \
    }

#define EXPECT_ELSE(c, ...)                                                                        \
    if(tok.type() != c) {                                                                          \
        log_error(__VA_ARGS__);                                                                    \
        tok.debug_print(std::cout) << std::endl;                                                   \
    } else

namespace kiwi {

/* Parsing function:
 *          // Sanity Check
            token = peek(); check(token);
            // --------------------------;

            // Parse logic

            // -------------------------- end
            check(last_token); consume();
            return result;
 */
class Parser {
  public:
    Parser(AbstractBuffer &buffer) : _lexer(buffer), builder(&ctx) { _lexer.consume(); }

    Token nexttok() {
        _lexer.consume();
        return _lexer.peek();
    }

    void consume_token() { _lexer.consume(); }
    Token peek_token() { return _lexer.peek(); }

    Dict<String, Option<Tuple<bool, uint8>>> operators = {
        {"+", Option<Tuple<bool, uint8>>(std::make_tuple(true, 0))},
        {"-", Option<Tuple<bool, uint8>>(std::make_tuple(true, 0))},
        {"*", Option<Tuple<bool, uint8>>(std::make_tuple(true, 0))},
        {"/", Option<Tuple<bool, uint8>>(std::make_tuple(true, 0))},
        {"return", Option<Tuple<bool, uint8>>(std::make_tuple(false, 0))}};

    /*/ fold_left
    Module parse_declarations(int i) {
        using InsertResult = std::pair<Module::iterator, bool>;
        Module my_module;

        while(peek_token().type() != tok_eof) {
            Tuple<String, Statement *> val = parse_declaration(i + 1);
            InsertResult result =
                my_module.insert(std::make_pair(std::get<0>(val), std::get<1>(val)));

            if(!result.second) {
                // TODO print reference to the previous declaration so user can check
                log_warn("Declaration `", std::get<0>(val), "` already exists");
            }
        }

        return my_module;
    }*/

    //*/ Top level declarations
    Tuple<String, Expression *> parse_declaration(int i) {
        log_cdebug(i, "");
        Token tok = peek_token();

        // predict next expression from current token
        switch(tok.type()) {
        case tok_macro:
            return parse_macro(i + 1);
        case tok_def:
            return parse_function(i + 1);
        case tok_union:
            return parse_record(i + 1);
        case tok_struct:
            return parse_record(i + 1);
        default:
            // parse_expression(i + 1)
            return std::make_tuple("__main__", nullptr);
        }

    } //*/

    Tuple<String, Expression *> parse_macro(int i) { return parse_function(i); }

    Expression *parse_statement(int i) { return parse(i); }

    Type *parse_type(int i) {
        Token tok = peek_token();
        if(tok.type() == tok_identifier)
            return builder.make_builtin(tok.identifier());

        return nullptr;
    }

    // <name>:<expr> <end_sep> <name>:<expr> <term_sep>
    Array<Tuple<String, Statement *>> parse_attribute_list(char end_sep, char term_sep, int i) {
        Array<Tuple<String, Statement *>> attributes;
        String name       = "";
        Statement *type   = nullptr;
        bool parsing_type = false;
        bool push_ready   = false;
        Token tok         = peek_token();

        while(tok.type() != term_sep && tok.type() != tok_eof) {
            tok = peek_token();

            if(!parsing_type && tok.type() == tok_identifier) {
                log_info("parsing: ", tok.identifier());
                name = tok.identifier();
            } else if(parsing_type) {
                type         = parse_type(i + 1);
                parsing_type = false;
            } else if(tok.type() == ':') {
                log_info("parsing type ");
                parsing_type = true;
                push_ready   = true;
            } else if(tok.type() == end_sep && push_ready) {
                log_info("pushing attributes");
                parsing_type = false;
                attributes.emplace_back(name, type);
                push_ready = false;
            }

            consume_token();
        }

        if(push_ready) {
            attributes.emplace_back(name, type);
        }

        EXPECT(term_sep, "expected end of attribute list");
        consume_token();
        return attributes;
    }

    Tuple<String, Expression *> parse_record(int i) {
        // ---------------------- Sanity Check --------------------------------
        log_cdebug(i, "");
        Token tok           = peek_token();
        bool parsing_struct = false;
        if(tok.type() != tok_struct && tok.type() != tok_union) {
            log_error("Expected Parsing a Record");
        }

        if(tok.type() == tok_struct) {
            parsing_struct = true;
        }
        // --------------------------------------------------------------------

        tok = nexttok();
        EXPECT(tok_identifier, "expected reccord/class/struct name");
        String name = tok.identifier();
        consume_token();

        // Parse Meta Types
        Array<Tuple<String, Statement *>> meta_types;

        tok = peek_token();
        if(tok.type() == '(') {
            log_info("Parsing MetaTypes");
            consume_token();
            meta_types = parse_attribute_list(',', ')', i + 1);
        }

        // --------------------------------------------------------------------
        tok = peek_token();
        EXPECT_ELSE(':', "expected ':' after user type declaration") { consume_token(); }

        tok = peek_token();
        EXPECT(tok_newline, "expected new line after user type declaration") { consume_token(); }
        // --------------------------------------------------------------------

        // Parse Attributes
        Array<Tuple<String, Statement *>> attributes =
            parse_attribute_list(tok_newline, tok_desindent, i + 1);

        // --------------------------------------------------------------------
        EXPECT(tok_desindent, "expected desindentation got '", tok_to_string(tok.type()), "'");
        consume_token();

        if(parsing_struct)
            return std::make_tuple(name, builder.make_struct(name, meta_types, attributes));
        return std::make_tuple(name, builder.make_union(name, meta_types, attributes));
    }

    Tuple<Array<String>, Array<Statement *>> parse_args(int i) {
        // ---------------------- Sanity Check --------------------------------
        log_cdebug(i, "");
        Token tok = peek_token();
        EXPECT('(', "expected '(' got '", tok.type(), "'");
        // --------------------------------------------------------------------

        Array<Statement *> arg_types;
        Array<String> arg_names;

        // FIXME missing )
        tok = nexttok();
        while(tok.type() != ')') {
            // handle the case where no values are present after a ','
            if(tok.type() == tok_identifier) {
                arg_names.push_back(tok.identifier());
                tok = nexttok();

                // Type annotation is optional
                if(tok.type() == ':') {
                    tok = nexttok();

                    EXPECT(tok_identifier, "expected type name");
                    log_info("Type is `", tok.identifier(), "`");
                    arg_types.push_back(builder.make_builtin(tok.identifier()));

                    tok = nexttok();
                }

                // Has next ?
                if(tok.type() == ',')
                    tok = nexttok();
            }
        }

        // --------------------------------------------------------------------
        EXPECT(')', "expected ')' got '", tok.type(), "'");
        consume_token();
        return std::make_tuple(arg_names, arg_types);
    }

    Tuple<String, Expression *> parse_function(int i) {
        // ---------------------- Sanity Check --------------------------------
        log_cdebug(i, "");
        Token tok = peek_token();

        if(tok.type() != tok_def && tok.type() != tok_macro) {
            log_error("not a function or a macro");
        }
        // --------------------------------------------------------------------

        // >>>>>>>> Parse Prototype
        tok = nexttok();
        EXPECT(tok_identifier, "expected function name");

        String name = tok.identifier();
        consume_token();
        Array<Statement *> arg_types;
        Array<String> arg_names;

        // >>>>>>>> Arguments
        // args = (identifier: identifier,*)
        std::tie(arg_names, arg_types) = parse_args(i);
        // <<<<<<<< Arguments

        // >>>>>>>> Return Type
        // -> return_type
        Expression *type = nullptr;
        tok              = peek_token();
        if(tok.type() == tok_arrow) {
            tok = nexttok();
            EXPECT(tok_identifier, "expected type name, got '", tok.type(), "'");
            log_info("Type is `", tok.identifier(), "`");

            // type = reinterpret_cast<Arrow<ASTTrait> *>(
            //    RBuilder::arrow(arg_types, RBuilder::builtin(tok.identifier())).take_ownership());

            consume_token();
        }
        // <<<<<<<< Return Type

        // >>>>>>>> Prototype end
        // :\n....
        tok = peek_token();
        EXPECT(':', "':' expected, got '", int(tok.type()), "'");
        tok = nexttok();
        EXPECT(tok_newline, "new line expected, got '", tok.type(), "'");
        tok = nexttok();
        EXPECT(tok_indent, "indent expected, got '", int(tok.type()), "'");
        consume_token();
        // <<<<<<<< Prototype end

        FunctionBuilder fbuilder = builder.make_function(name);
        if(arg_names.size() != arg_types.size()) {
            log_info("Argument name size != Argument Type size");
        } else {
            for(std::size_t i = 0; i < arg_names.size(); ++i) {
                fbuilder.add_arg(arg_names[i], arg_types[i]);
            }
        }

        Block *b = new Block();
        b->statements.push_back(parse(i + 1));

        fbuilder.add_body(b);

        Function *fun = fbuilder.build();
        // --------------------------------------------------------------------
        EXPECT(tok_desindent, "expected desindentation got '", tok.type(), "'");
        consume_token();
        return std::make_tuple(name, fun);
    }

    Expression *parse_operator(int i, String const &op, Tuple<bool, uint8> const &info) {
        Token tok = peek_token();
        // log_error(tok.type(), tok.identifier());

        // if binary
        if(std::get<0>(info)) {
        }

        consume_token();
        Expression *expr = parse(i + 1);
        return builder.make_unary_call(op, expr);
    }

    Expression *parse(int i) { return parse_expression(i); }

    Expression *parse_expression(int i) {
        log_cdebug(i, "");

        Expression *lhs = nullptr;
        Token tok       = peek_token();

        log_trace(tok);

        if(tok.type() == '(') {
            consume_token();
            lhs = parse(i + 1);
            tok = peek_token();

            if(tok.type() != ')') {
                log_error("Expected closing parenthesis");
            }
        } else if(tok.type() == tok_float)
            lhs = parse_value(i + 1, tok.as_float());

        else if(tok.type() == tok_int)
            lhs = parse_value(i + 1, tok.as_integer());

        else if(tok.type() == tok_identifier) {
            Option<Tuple<bool, uint8>> data = operators[tok.identifier()];

            if(data.is_defined()) {
                return parse_operator(i + 1, tok.identifier(), data.get());
            }

            // log_error("'", tok.type(), "' '", tok.identifier(), "'");
            lhs = parse_identifier(i + 1, tok.identifier());
        }

        consume_token(); // Eat ')'/tok_int...
        tok = peek_token();

        log_trace(tok);
        tok.debug_print(std::cout) << std::endl;
        // end of expr
        if(tok.type() == tok_eof || tok.type() == ')')
            return lhs;

        // operator
        if(tok.type() == '+') {
            log_error(tok.type(), tok.identifier());
            consume_token();
            return parse_add(i + 1, lhs);
        } // FIXME
        else {
            log_trace(tok);
            tok.debug_print(std::cout) << std::endl;
            log_error("`", int(tok.type()), "` is not a correct op (id=", tok.identifier(), ")");

            if(lhs == nullptr) {
                log_error("nullptr");
            } else {
                // print_expr(std::cout, lhs);
                // std::cout << std::endl;
            }
        }

        return lhs;
    }

    Expression *parse_add(int i, Expression *lhs) {
        log_cdebug(i, "");
        Expression *rhs = parse(i + 1);

        if(rhs == nullptr)
            return lhs;

        return builder.make_binary_call("+", lhs, rhs);
        // Builder::add(lhs, rhs);
    }

    Expression *parse_value(int i, double val) {
        log_cdebug(i, "");
        return builder.make_value(val);
        // Builder::value(val);
    }

    Expression *parse_identifier(int i, std::string name) {
        log_cdebug(i, "");
        return builder.make_placeholder(name);
    }

  private:
    Lexer _lexer;
    BuilderContext ctx;
    Builder builder;
};

} // namespace kiwi
