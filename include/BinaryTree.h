#ifndef BINARYTREE_H
#define BINARYTREE_H

#include <string>
#include <sstream>

template <class T>
class BinaryTree {
private:
    struct Node {
        T val;
        Node* left;
        Node* right;
    };

    Node* head;
    int (*cmp)(T, T);
    /*
     * -1 - a < b
     * 0  - a = b
     * 1  - a > b
     */

    void DeleteNode(Node* node) {
        if (node->left) DeleteNode(node->left);
        if (node->right) DeleteNode(node->right);
        delete node;
    }

    void Traverse(Node* node, void (*func)(T), const char* order) {
        for (int index = 0; index < 3; index++) {
            switch (order[index]) {
            case 'L':
                if (node->left) Traverse(node->left, func, order);
                break;
            case 'R':
                if (node->right) Traverse(node->right, func, order);
                break;
            default:
                func(node->val);
                break;
            }
        }
    }
public:
    BinaryTree(int (*compare)(T, T)): head(nullptr), cmp(compare) {}

    ~BinaryTree() {
        if (head) DeleteNode(head);
    }

    void Insert(T item) {
        Node node = {item, nullptr, nullptr};
        if (head == nullptr) {
            head = new Node(node);
            return;
        }
        Node* parent = head;
        int compared;
        bool finished = false;
        while (!finished) {
            compared = cmp(item, parent->val);
            switch (compared) {
            case -1:
                finished = parent->left == nullptr;
                if (!finished) parent = parent->left;
                break;
            case 1:
                finished = parent->right == nullptr;
                if (!finished) parent = parent->right;
                break;
            default:
                return;  // 0 - повторяющееся значение
            }
        }
        if (compared == -1) {
            parent->left = new Node(node);
        } else {
            parent->right = new Node(node);
        }
    }

    bool Search(T item) {
        Node* lookAt = head;
        while (lookAt) {
            if (cmp(item, lookAt->val) == 0) return true;
            lookAt = cmp(item, lookAt->val) == -1 ? lookAt->left : lookAt->right;
        }
        return false;
    }

    BinaryTree<T> Map(T (*func)(T), const char* order) {
        BinaryTree<T> result = BinaryTree<T>(cmp);
        auto append = [&](T val) {result.Insert(func(val));};
        Traverse(head, append, order);
        return result;
    }

    BinaryTree<T> Where(bool (*func)(T), const char* order) {
        BinaryTree<T> result = BinaryTree<T>(cmp);
        auto append = [&](T val) {if (func(val)) result.Insert(val);};
        Traverse(head, append, order);
        return result;
    }

    T Reduce(T (*func)(T, T), T initial, const char* order) {
        auto calculate = [&](T val) {initial = func(initial, val);};
        Traverse(head, calculate, order);
        return initial;
    }

    std::string ToString(std::string (*toText)(T), const char* order) {
        std::string text;
        std::ostringstream textOut(text);
        auto output = [&](T val) {textOut << toText(val);};
        Traverse(head, textOut, order);
        return text;
    }
};

#endif // BINARYTREE_H
