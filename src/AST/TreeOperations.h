#pragma once
#include <ostream>
#include <unordered_map>


namespace kiwi{

// TODO: A better context would be Dict<string, Expr*>
typedef std::unordered_map<std::string, Expression*> Context;

class Expression;
class Function;


void print(std::ostream& out, Expression* expr);

// void append_args(Function* fun, Expression* expr);

// Compute the graph (all placeholder must be defined)
double full_eval(const Context& ctx, Expression* expr);

// free memory
void free(Expression* expr);

// Copy a tree
Expression* copy(Expression* expr, bool keep_borrowed = false);

}
