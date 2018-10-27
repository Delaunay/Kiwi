#ifndef KIWI_STACK_HEADER_H_
#define KIWI_STACK_HEADER_H_

#include "../Types.h"

namespace kiwi {

template <typename T> class Stack {
  public:
    Stack(std::size_t reserve_size = 4) { _vector.reserve(reserve_size); }

    void push(T const &v) { _vector.push_back(v); }

    T pop() {
        T item = _vector.back();
        _vector.pop_back();
        return item;
    }

    void pop_back() { _vector.pop_back(); }

    std::size_t size() { return _vector.size(); }

    T &last() { return _vector[_vector.size() - 1]; }

  private:
    Array<T> _vector;
};
} // namespace kiwi
#endif
