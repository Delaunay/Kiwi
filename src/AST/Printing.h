#pragma once
#include <ostream>
#include <unordered_map>

typedef std::unordered_map<std::string, double> Context;

class Expression;

namespace kiwi{

void print(std::ostream& out, Expression* expr);
double full_eval(const Context& ctx, Expression* expr);
}
