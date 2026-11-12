#ifndef ISET_H
#define ISET_H

template <class T>
class ISet {
public:
    virtual int GetCount() = 0;
    virtual bool IsEmpty();
    virtual bool HasElement(T element) = 0;
    virtual bool Contains(ISet<T>* set) = 0;

    virtual ISet<T>* Union(ISet<T>* set) = 0;
    virtual ISet<T>* Intersect(ISet<T>* set) = 0;
    virtual ISet<T>* Difference(ISet<T>* set) = 0;

    virtual bool operator==(ISet<T>* set) = 0;
};

template <class T>
bool ISet<T>::IsEmpty() {
    return GetCount() == 0;
}

#endif // ISET_H
