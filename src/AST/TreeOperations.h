#pragma once
#include <ostream>
#include <unordered_map>


namespace kiwi{

typedef std::unordered_map<std::string, double> Context;

class Expression;

void print(std::ostream& out, Expression* expr);

double full_eval(const Context& ctx, Expression* expr);

void free(Expression* expr);

Expression* copy(Expression* expr);

}
