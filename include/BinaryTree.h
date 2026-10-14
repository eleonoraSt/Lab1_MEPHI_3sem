#ifndef BINARYTREE_H
#define BINARYTREE_H

#include <string>
#include <sstream>
#include <stdexcept>

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

    Node* SearchNode(Node* lookAt, T item) {
        while (lookAt) {
            if (cmp(item, lookAt->val) == 0) return lookAt;
            lookAt = cmp(item, lookAt->val) == -1 ? lookAt->left : lookAt->right;
        }
        return nullptr;
    }

    bool IsContainsFromHead(Node* lookAt, Node* lookFor) {
        bool contains = cmp(lookAt->val, lookFor->val) == 0;
        if (!contains) return false;
        if (lookFor->left != nullptr) {
            if (lookAt->left == nullptr) return false;  // нужна левая ветвь, а её нет
            contains = IsContainsFromHead(lookAt->left, lookFor->left);
            if (!contains) return false;
        }
        if (lookFor->right != nullptr) {
            if (lookAt->right == nullptr) return false;  // нужна левая ветвь, а её нет
            contains = IsContainsFromHead(lookAt->right, lookFor->right);
            if (!contains) return false;
        }
        return true;
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
        return SearchNode(head, item) != nullptr;
    }

    BinaryTree<T> SubTree(T root) {
        Node* rootNode = SearchNode(head, root);
        if (rootNode == nullptr) throw std::invalid_argument("No root with given value");
        BinaryTree<T> result = BinaryTree<T>(cmp);
        auto append = [&](T val) {result.Insert(val);};
        Traverse(rootNode, append, "LCR");  // обход неважен
        return result;
    }

    bool IsSubTree(BinaryTree<T>& subtree) {
        Node* root = SearchNode(head, subtree.head->val);
        if (root == nullptr) return false;  // вершина данного не входит сюда
        return IsContainsFromHead(root, subtree.head);
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

    void FromString(std::string text, T (*fromText)(std::string)) {
        std::istringstream textIn(text);
        std::string buf;
        T val;
        while (textIn >> buf) {
            val = fromText(buf);
            Insert(val);
        }
    }
};

#endif // BINARYTREE_H
