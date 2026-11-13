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

    MutableSet(MutableSet<T>& set): tree(set.tree) {}

    int GetCount() {
        return tree.GetCount();
    }

    bool HasElement(T element) {
        return tree.Search(element);
    }

    bool IsSubsetOf(ISet<T>* set) {
        auto isElement = [&](bool initial, T item) {return initial && set->HasElement(item);};
        return tree.Reduce(isElement, true, "LCR");  // обход неважен
    }

    ISet<T>* Map(T (*func)(T)) {
        MutableSet<T>* mapped = new MutableSet<T>();
        mapped->tree = tree.Map(func, "LCR");  // обход неважен
        return mapped;
    }

    ISet<T>* Where(bool (*func)(T)) {
        MutableSet<T>* sorted = new MutableSet<T>();
        sorted->tree = tree.Where(func, "LCR");  // обход неважен
        return sorted;
    }

    T Reduce(T (*func)(T, T), T initial) {
        return tree.Reduce(func, initial, "LCR");  // обход неважен
    }

    bool operator==(ISet<T>& set) {
        if (GetCount() != set.GetCount()) return false;
        return IsSubsetOf(&set);
    }
};

#endif // MUTABLESET_H
