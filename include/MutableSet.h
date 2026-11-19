#ifndef MUTABLESET_H
#define MUTABLESET_H

#include <sstream>
#include "BinaryTree.h"
#include "ISet.h"

template <class T>
class MutableSet: public ISet<T> {
private:
    UnqPtr<BinaryTree<T>> tree;
public:
    MutableSet(): tree(make_unq(BinaryTree<T>())) {}

    MutableSet(MutableSet<T>& set): tree(make_unq(BinaryTree<T>(*set.tree))) {}

    MutableSet(MutableSet<T>&& set): tree(std::move(set.tree)) {}

    int GetCount() {
        auto count = [&](int num, T item) {return ++num;};
        return tree->Reduce(count, 0, ARBITRARY_ORDER);
    }

    bool HasElement(T element) {
        return tree->Search(element);
    }

    bool IsSubsetOf(ISet<T>* set) {
        auto isElement = [&](bool initial, T item) {return initial && set->HasElement(item);};
        return tree->Reduce(isElement, true, ARBITRARY_ORDER);
    }

    ISet<T>* Union(ISet<T>* set) {
        auto add = [&](T initial, T item) {
            tree->Insert(item);
            return initial;
        };
        set->Reduce(add, T());  // reduce, чтобы не создавалось новое дерево
        return this;
    }

    ISet<T>* Intersect(ISet<T>* set) {
        auto isInIntersection = [&](T item) {return set->HasElement(item);};
        UnqPtr<BinaryTree<T>> intersection = make_unq(tree->Where(isInIntersection, ARBITRARY_ORDER));
        tree = std::move(intersection);
        return this;
    }

    ISet<T>* Difference(ISet<T>* set) {
        auto remove = [&](T initial, T item) {
            tree->Delete(item);
            return initial;
        };
        set->Reduce(remove, T());  // reduce, чтобы не создавалось новое дерево
        return this;
    }

    ISet<T>* Map(T (*func)(T)) {
        MutableSet<T>* mapped = new MutableSet<T>();
        mapped->tree = tree->Map(func, ARBITRARY_ORDER);
        return mapped;
    }

    ISet<T>* Where(bool (*func)(T)) {
        MutableSet<T>* sorted = new MutableSet<T>();
        sorted->tree = tree->Where(func, ARBITRARY_ORDER);
        return sorted;
    }

    T Reduce(T (*func)(T, T), T initial) {
        return tree->Reduce(func, initial, ARBITRARY_ORDER);
    }

    std::string ToString(std::string (*toText)(T)) {
        return tree->ToString(toText, ARBITRARY_ORDER);
    }

    void FromString(std::string text, T (*fromText)(T)) {
        tree->FromString(text, fromText);
    }

    MutableSet<T> operator=(MutableSet<T>& set) {
        auto identical = [&](T val) {return val;};
        tree = set.tree->Map(identical, ARBITRARY_ORDER);
    }

    MutableSet<T> operator=(MutableSet<T>&& set) {
        tree = std::move(set.tree);
    }

    bool operator==(ISet<T>& set) {
        if (GetCount() != set.GetCount()) return false;
        return IsSubsetOf(&set);
    }
};

#endif // MUTABLESET_H
