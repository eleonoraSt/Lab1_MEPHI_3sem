#ifndef UNQPTR_H
#define UNQPTR_H

#include <stdexcept>  // ошибки при nullptr

template <class T>
class UnqPtr {
private:
    T* ptr;
    bool isArray;

    template <class U>
    friend class UnqPtr;

    void clear() {
        if (isArray) {delete[] ptr;} else {delete ptr;}
    }
public:
    UnqPtr(): ptr(nullptr), isArray(false) {}

    UnqPtr(const UnqPtr<T>& other) = delete;

    UnqPtr(UnqPtr<T>&& other): ptr(other.ptr), isArray(other.isArray) {
        other.ptr = nullptr;
    }

    template <class Derived>
    UnqPtr(UnqPtr<Derived>&& other) {
        static_assert(std::is_base_of<T, Derived>::value, "Argument must be of the target class or derived from it");
        ptr = static_cast<T*>(other.ptr);
        isArray = other.isArray;
        other.ptr = nullptr;
    }

    ~UnqPtr() {clear();}

    UnqPtr<T>& operator=(const UnqPtr<T>& other) = delete;

    UnqPtr<T>& operator=(UnqPtr<T>&& other) {
        if (&other == this) return *this;
        clear();
        ptr = other.ptr;
        isArray = other.isArray;
        other.ptr = nullptr;
        other.isArray = false;
        return *this;
    }

    template <class Derived>
    UnqPtr<T>& operator=(UnqPtr<Derived>&& other) {
        static_assert(std::is_base_of<T, Derived>::value, "Argument must be of the target class or derived from it");
        clear();
        ptr = static_cast<T*>(other.ptr);
        isArray = other.isArray;
        other.ptr = nullptr;
        return *this;
    }

    T& operator*() const {
        if (isArray) throw std::invalid_argument("-> not defined for array unique pointers");
        if (ptr == nullptr) throw std::invalid_argument("nullptr unique pointer");
        return *ptr;
    }

    T* operator->() const {
        if (isArray) throw std::invalid_argument("-> not defined for array unique pointers");
        if (ptr == nullptr) throw std::invalid_argument("nullptr unique pointer");
        return ptr;
    }

    T& operator[](size_t index) const {
        if (!isArray) throw std::invalid_argument("[] not defined for non-array unique pointers");
        if (ptr == nullptr) throw std::invalid_argument("nullptr unique pointer");
        return ptr[index];
    }

    bool isNull() {
        return ptr == nullptr;
    }
};

template <class T, class... Args>
UnqPtr<T> make_unq(Args&&... args) {
    return UnqPtr<T>(new T(args...), false);
}

template <class T>
UnqPtr<T> make_unq_array(size_t size) {
    return UnqPtr<T>(new T[size], true);
}

#endif // UNQPTR_H
