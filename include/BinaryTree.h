#ifndef BINARYTREE_H
#define BINARYTREE_H

#include <string>
#include <sstream>
#include <stdexcept>

#include "OrderStr.h"

template <class T>
class BinaryTree {
private:
    struct Node {
        T val;
        Node* left;
        Node* right;
        Node* thread;
    };

    Node* head;
    int (*cmp)(T, T);
    const char* threadOrder;
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

    void TraverseNoThread(Node* node, void (*func)(T), const char* order) {
        for (int index = 0; index < 3; index++) {
            switch (order[index]) {
            case 'L':
                if (node->left) TraverseNoThread(node->left, func, order);
                break;
            case 'R':
                if (node->right) TraverseNoThread(node->right, func, order);
                break;
            default:
                func(node->val);
                break;
            }
        }
    }

    void TraverseThread(void (*func)(T)) {
        Node* node;
        switch (threadOrder[0]) {
        case 'L':
            node = (ParentOfMostLeft(head))->left;
        case 'R':
            node = (ParentOfMostRight(head))->right;
        default:
            node = head;
        }
        func(node->val);
        while (node->thread != nullptr) {
            node = node->thread;
            func(node->val);
        }
    }

    void Traverse(void (*func)(T), const char* order) {
        if (threadOrder == nullptr || order && !equal(threadOrder, order)) {
            if (order == nullptr) throw std::invalid_argument("No traversal order given");
            TraverseNoThread(head, func, order);
        } else {
            TraverseThread(func);
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
            if (lookAt->right == nullptr) return false;  // нужна правая ветвь, а её нет
            contains = IsContainsFromHead(lookAt->right, lookFor->right);
            if (!contains) return false;
        }
        return true;
    }

    Node* ParentOfMostLeft(Node* lookAt) {  // нужно в алгоритме балансировки
        if (lookAt->left == nullptr) throw std::invalid_argument("Given node is a leaf");
        while (lookAt->left->left != nullptr) lookAt = lookAt->left;
        return lookAt;
    }

    Node* ParentOfMostRight(Node* lookAt) {  // нужно в алгоритме балансировки
        if (lookAt->right == nullptr) throw std::invalid_argument("Given node is a leaf");
        while (lookAt->right->right != nullptr) lookAt = lookAt->right;
        return lookAt;
    }

    int BalanceNode(Node* parent, Node* lookAt) {  // возвращает высоту ветки
        int heightLeft = lookAt->left == nullptr ? 0 : BalanceNode(lookAt, lookAt->left);
        int heightRight = lookAt->right == nullptr ? 0 : BalanceNode(lookAt, lookAt->right);
        while (heightLeft - heightRight > 1) {
            Node* parentOfNewRoot = ParentOfMostRight(lookAt->left);
            Node* buf = lookAt->left;
            lookAt->left = parentOfNewRoot->right->left;
            parentOfNewRoot->right->left = buf;
            parentOfNewRoot->right->right = lookAt;
            if (parent->left == lookAt) {
                parent->left = parentOfNewRoot->right;
            } else {
                parent->right = parentOfNewRoot->right;
            }
            parentOfNewRoot->right = nullptr;
            heightLeft--;
            heightRight++;
        }
        while (heightRight - heightLeft > 1) {
            Node* parentOfNewRoot = ParentOfMostLeft(lookAt->right);
            Node* buf = lookAt->right;
            lookAt->right = parentOfNewRoot->left->right;
            parentOfNewRoot->left->right = buf;
            parentOfNewRoot->left->left = lookAt;
            if (parent->left == lookAt) {
                parent->left = parentOfNewRoot->left;
            } else {
                parent->right = parentOfNewRoot->left;
            }
            parentOfNewRoot->left = nullptr;
            heightRight--;
            heightLeft++;
        }
        return heightLeft > heightRight ? ++heightLeft: ++heightRight;
    }

    void ToStringPair(Node* lookAt, std::ostringstream* textOut, std::string (*toText)(T)) {
        // Формат: "{parent : child}, "
        if (lookAt->left) {
            *textOut << "{" + toText(lookAt->val) + " : " + toText(lookAt->left->val) << "}, ";
            ToStringPair(lookAt->left, textOut, toText);
        }
        if (lookAt->right) {
            *textOut << "{" + toText(lookAt->val) + ": " + toText(lookAt->right->val) << "}, ";
            ToStringPair(lookAt->right, textOut, toText);
        }
    }
public:
    BinaryTree(int (*compare)(T, T)): head(nullptr), cmp(compare), threadOrder(nullptr) {}

    BinaryTree(int (*compare)(T, T), const char* order): head(nullptr), cmp(compare), threadOrder(order) {}

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
        Traverse(append, "LCR");  // обход неважен
        return result;
    }

    bool IsSubTree(BinaryTree<T>& subtree) {
        Node* root = SearchNode(head, subtree.head->val);
        if (root == nullptr) return false;  // вершина данного не входит сюда
        return IsContainsFromHead(root, subtree.head);
    }

    void Balance() {
        int heightLeft = head->left == nullptr ? 0 : BalanceNode(head, head->left);
        int heightRight = head->right == nullptr ? 0 : BalanceNode(head, head->right);
        while (heightLeft - heightRight > 1) {
            Node* parentOfNewRoot = ParentOfMostRight(head->left);
            Node* buf = head->left;
            head->left = parentOfNewRoot->right->left;
            parentOfNewRoot->right->left = buf;
            parentOfNewRoot->right->right = head;
            head = parentOfNewRoot->right;
            parentOfNewRoot->right = nullptr;
            heightLeft--;
            heightRight++;
        }
        while (heightRight - heightLeft > 1) {
            Node* parentOfNewRoot = ParentOfMostLeft(head->right);
            Node* buf = head->right;
            head->right = parentOfNewRoot->left->right;
            parentOfNewRoot->left->right = buf;
            parentOfNewRoot->left->left = head;
            head = parentOfNewRoot->left;
            parentOfNewRoot->left = nullptr;
            heightRight--;
            heightLeft++;
        }
    }

    BinaryTree<T> Map(T (*func)(T), const char* order=nullptr) {
        BinaryTree<T> result = BinaryTree<T>(cmp);
        auto append = [&](T val) {result.Insert(func(val));};
        Traverse(append, order);
        return result;
    }

    BinaryTree<T> Where(bool (*func)(T), const char* order=nullptr) {
        BinaryTree<T> result = BinaryTree<T>(cmp);
        auto append = [&](T val) {if (func(val)) result.Insert(val);};
        Traverse(append, order);
        return result;
    }

    T Reduce(T (*func)(T, T), T initial, const char* order=nullptr) {
        auto calculate = [&](T val) {initial = func(initial, val);};
        Traverse(calculate, order);
        return initial;
    }

    std::string ToString(std::string (*toText)(T), const char* order=nullptr) {
        std::string text;
        std::ostringstream textOut(text);
        auto output = [&](T val) {textOut << toText(val);};
        Traverse(textOut, order);
        return text;
    }

    std::string ToStringInPairs(std::string (*toText)(T)) {
        std::string text;
        std::ostringstream textOut(text);
        ToStringPair(head, &textOut, toText);
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

    void FromStringInPairs(std::string text, T (*fromText)(std::string)) {
        int length = text.size();
        int bufLength;
        std::istringstream textIn(text);
        std::string buf;
        T val;
        while (!textIn.eof()) {
            textIn.ignore(length, '{');
            if (!(textIn >> buf)) return;  // строка кончилась
            if (head == nullptr) {
                val = fromText(buf);
                Insert(val);
            }
            textIn >> buf;
            if (buf != ":") throw std::invalid_argument("Incorrect string format");
            if (!(textIn >> buf)) throw std::invalid_argument("Incorrect string format");
            bufLength = buf.size();
            if (buf[bufLength - 2] != '}' || buf[bufLength - 1] != ',') {
                throw std::invalid_argument("Incorrect string format");
            }
            buf.erase(bufLength - 2, bufLength - 1);
            val = fromText(buf);
            Insert(val);
        }
    }
};

#endif // BINARYTREE_H
