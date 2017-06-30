#pragma once
#include <ostream>
#include <unordered_map>


namespace kiwi{

// TODO: A better context would be Dict<string, Expr*>
typedef std::unordered_map<std::string, class Expression*> Context;

std::ostream& print_expr(std::ostream& out, class Expression* expr);

// void append_args(Function* fun, Expression* expr);

// Compute the graph (all placeholder must be defined)
double full_eval(const Context& ctx, class Expression* expr);

// free memory
void free(class Expression* expr);

// Copy a tree
class Expression* copy(class Expression* expr, bool keep_borrowed = false);

}
