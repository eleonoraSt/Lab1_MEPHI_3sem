#ifndef MUTABLESET_H
#define MUTABLESET_H

#include "BinaryTree.h"
#include "ISet.h"

template <class T>
class MutableSet: public ISet<T> {
private:
    BinaryTree<T> tree;
public:
    MutableSet(): tree(BinaryTree<T>()) {}

    int GetCount() {
        return tree.GetCount();
    }

    bool HasElement(T element) {
        return tree.Search(element);
    }

    bool IsSubsetOf(ISet<T>* set) {
        auto isElement = [&](bool initial, T item) {return initial && set->HasElement(item);};
        return tree.Reduce(isElement, true, "LCR");  // обход
    }
};

#endif // MUTABLESET_H
