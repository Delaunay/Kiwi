#pragma once

#include <cstdlib>

//#define KIWI_DEBUG
#include "../Debug.h"
#include <iostream>

namespace kiwi {

class Expression;

/* Root represent the root of our tree, when a node is created
 * it is at first created as root then we can combine roots
 * to make a bigger tree. once combined the new root takes
 * ownership of the two children tree.
 *
 * The root has the responsibility of freeing its children.
 *
 * Root is kind of a unique_ptr but that allows copying
 * and it was adapted for our tree structure.
 */
namespace generic {
template <typename T> class Root {
  public:
    typedef T *ptr_type;

    Root(T *ptr = nullptr) : _root(ptr), _owned(false) {}

    Root(const Root &r) : _root(r.take_ownership()), _owned(false) {}

    Root(const Root &&r) : _root(r.take_ownership()), _owned(false) {}

    Root &operator=(const Root &&r) {
        if(_root == nullptr) {
            _owned = false;
            _root  = r.take_ownership();
        } else {
            // FIXME
            std::cout << "Root already owning a node\n";
        }
        return *this;
    }

    ~Root() {
        if(!_owned) {
            // freeprint(_root);
            // free(_root);
        }
    }

    /* Root is inserted inside a new tree, the parent
     * takes ownership of the node. If the node is already
     * owned a copy of the tree is returned instead
     */
    T *take_ownership(bool borrow = false) const {
        if(_owned) {
            // FIXME
            // printdt("Node already owned, copying subtree: ", _root);
            // if (borrow)
            //    return;
            // return copy(_root);
        }

        _owned = true;
        return _root;
    }

    bool owned() const { return _owned; }

    T *get() const { return _root; }
    T *operator->() const { return _root; }

    operator T *() const { return _root; }

  private:
    T *_root;
    mutable bool _owned;
};

/* Used to create trees internaly when the created tree
 * cannot be owned by anyother one. (example when making a copy)
 * As opposed to Root this can be trivially simplified to a naked pointer
 * by the compiler.
 */
template <typename T> class DummyRoot {
  public:
    typedef T *ptr_type;

    DummyRoot(T *ptr) : _root(ptr) {}

    T *take_ownership() const { return _root; }

    bool owned() const { return false; }

    T *get() const { return _root; }
    T *operator->() const { return _root; }

    operator T *() const { return _root; }

  private:
    T *_root;
};
} // namespace generic
} // namespace kiwi
