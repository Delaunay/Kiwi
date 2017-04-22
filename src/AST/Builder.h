#pragma once

#include <cmath>
#include "Expression.h"
#include "TreeOperations.h"

namespace kiwi {

/* Root represent the root of our tree, when a node is created
 * it is at first created as root then we can combine roots
 * to make a bigger tree. once combined the new root takes
 * ownership of the two children tree.
 *
 * The root has the responsibility of freeing its children.
 */
class Root{
public:
    Root(Expression* ptr):
        _root(ptr), _owned(false)
    {}

    Root(Root& r):
        _root(r._root), _owned(true)
    {}

    Root(Root&& r):
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
    Expression* take_ownership(){
        if (_owned){
            // FIXME
            std::cout << "Node already owned, copying subtree\n";
            return copy(_root);
        }

        _owned = true;
        return _root;
    }

    bool owned(){ return _owned; }

    Expression* get       (){ return _root; }
    Expression* operator->(){ return _root; }

    operator Expression*(){
        return _root;
    }

private:
    Expression* _root;
    bool        _owned;
};

class Builder{
public:
    static Root add(Root& lhs, Root& rhs){

        // Constant folding
        if (lhs->tag == NodeTag::value){
            Value* vlhs = static_cast<Value*>(lhs.get());
            if (std::abs(vlhs->value) <= 1e-12){
                return rhs;
            }

            if (rhs->tag == NodeTag::value){
                Value* vrhs = static_cast<Value*>(rhs.get());
                return new Value(vrhs->value + vlhs->value);
            }
        }

        if (rhs->tag == NodeTag::value){
            Value* vrhs = static_cast<Value*>(rhs.get());
            if (std::abs(vrhs->value) <= 1e-12){
                return lhs;
            }
        }

        Root root = new Add(lhs.take_ownership(),
                            rhs.take_ownership());

        PARENT(lhs->parent = root);
        PARENT(rhs->parent = root);
        return root;
    }

    static Root value(double value){
        return new Value(value);
    }

    static Root placeholder(const std::string& name){
        return new Placeholder(name);
    }
};


}
