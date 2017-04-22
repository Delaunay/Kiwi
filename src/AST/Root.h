#pragma once

#include <cstdlib>

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
class Root{
public:
    Root(Expression* ptr):
        _root(ptr), _owned(false)
    {}

    Root(const Root& r):
        _root(r._root), _owned(true)
    {}

    Root(const Root&& r):
        _root(r._root), _owned(false)
    {
        r.take_ownership();
    }

    ~Root(){
        if (!_owned){
            free(_root);
        }
    }

    /* Root is inserted inside a new tree, the parent
     * takes ownership of the node. If the node is already
     * owned a copy of the tree is returned instead
     */
    Expression* take_ownership() const{
        if (_owned){
            // FIXME
            std::cout << "Node already owned, copying subtree\n";
            return copy(_root);
        }

        _owned = true;
        return _root;
    }

    bool owned() const{ return _owned; }

    Expression* get       () const{ return _root; }
    Expression* operator->() const{ return _root; }

    operator Expression*() const{
        return _root;
    }

private:
    Expression*  _root;
    mutable bool _owned;
};

/* Used to create trees internaly when the created tree
 * cannot be owned by anyother one. (example when making a copy)
 * As opposed to Root this can be trivially simplified to a naked pointer
 * by the compiler.
 */
class DummyRoot{
public:
    DummyRoot(Expression* ptr):
        _root(ptr)
    {}

    Expression* take_ownership() const{
        return _root;
    }

    bool owned() const{ return false; }

    Expression* get       () const{ return _root; }
    Expression* operator->() const{ return _root; }

    operator Expression*() const{
        return _root;
    }

private:
    Expression* _root;
};

}
