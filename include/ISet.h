#ifndef ISET_H
#define ISET_H

#include <string>

template <class T>
class ISet {
public:
    virtual int GetCount() = 0;
    virtual bool IsEmpty();
    virtual bool HasElement(T element) = 0;
    virtual bool IsSubsetOf(ISet<T>* set) = 0;

    virtual ISet<T>* Union(ISet<T>* set) = 0;
    virtual ISet<T>* Intersect(ISet<T>* set) = 0;
    virtual ISet<T>* Difference(ISet<T>* set) = 0;

    virtual ISet<T>* Map(T (*func)(T)) = 0;
    virtual ISet<T>* Where(bool (*func)(T)) = 0;
    virtual T Reduce(T (*func)(T, T), T initial) = 0;

    virtual std::string ToString(std::string (*toText)(T)) = 0;
    virtual void FromString(std::string text, T (*fromText)(T)) = 0;

    virtual bool operator==(ISet<T>& set) = 0;
};

template <class T>
bool ISet<T>::IsEmpty() {
    return GetCount() == 0;
}

#endif // ISET_H
