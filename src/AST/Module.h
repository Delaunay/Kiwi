#ifndef KIWI_AST_MODULE_HEADER
#define KIWI_AST_MODULE_HEADER

#include "Definition.h"
#include "Expression.h"
#include "Stack.h"

namespace kiwi {

using Module = Dict<String, Definition *>;

using Context = Dict<String, Expression *>;

class LocalScope;

// This is the Top Scope that manages the memory
// Not Thread safe as new var are pushed forward
class GlobalScope {
  public:
    GlobalScope(std::size_t reserve_size) : _scope(reserve_size) {}

    void push(void()) {}

    template <typename... Args> void push(Expression *const &v, Args... args) {
        _scope.push(v);
        push(args...);
    }

    Expression *pop() { return _scope.pop(); }

    void pop_back() { _scope.pop_back(); }

    std::size_t size() { return _scope.size(); }

  private:
    void inc() { _depth += 1; }
    void dec() { _depth -= 1; }

    // size can be different from _scope
    Stack<Expression *> _scope;
    std::size_t _depth = 0;

    friend LocalScope new_scope(GlobalScope &scope);
    friend LocalScope new_scope(LocalScope &scope);

    friend class LocalScope;
};

// This is a Subscope that pushes things into thr main scope
// everything that the scope pushed is destroyed when the Subscope is destroyed
// It can be nested as much as needed
// A Call Expression create a subscope with the function's parameters
//
//
//   def call_function(function, args, parent_scope):
//      SubScope s(parent_scope);
//
//      for each arg in args:
//          s.push(arg)
//
//      return eval(function, s)
//      // args are automatically pop'ed from the scope
//

class LocalScope {
  public:
    LocalScope(LocalScope &scope) : _scope(scope._scope), _starting_size(scope.size()) {
        _scope.inc();
    }

    LocalScope(LocalScope &&scope) : _scope(scope._scope), _starting_size(scope.size()) {
        _scope.inc();
    }

    LocalScope(GlobalScope &scope) : _scope(scope), _starting_size(scope.size()) { _scope.inc(); }

    ~LocalScope() {
        for(std::size_t i = _starting_size, n = size(); i < n; ++i)
            _scope.pop_back();
        _scope.dec();
    }

    template <typename... Args> void push(Args... args) { _scope.push(args...); }

    Expression *pop() { return _scope.pop(); }

    std::size_t size() { return _scope.size(); }

  private:
    GlobalScope &_scope;
    std::size_t _starting_size;

    friend LocalScope new_scope(GlobalScope &scope);
    friend LocalScope new_scope(LocalScope &scope);
};

} // namespace kiwi

#endif
