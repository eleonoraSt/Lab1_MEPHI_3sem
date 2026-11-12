#ifndef BINARYTREE_H
#define BINARYTREE_H

#include <string>
#include <sstream>
#include <stdexcept>
#include "UnqPtr.h"

template <class T>
class BinaryTree {
private:
    struct Node {
        T val;
        UnqPtr<Node> left;
        UnqPtr<Node> right;
    };

    UnqPtr<Node> head;
    int (*cmp)(T, T);
    /*
     * -1 - a < b
     * 0  - a = b
     * 1  - a > b
     */

    UnqPtr<Node>& SearchNode(T item, UnqPtr<Node>& root) {
        UnqPtr<Node>* address = &root;
        int compared;
        while (!(address->isNull())) {
            compared = cmp(item, (*address)->val);
            if (compared == 0) return *address;
            address = compared == -1 ? &((*address)->left) : &((*address)->right);
        }
        return UnqPtr<Node>();  // не нашлось
    }

    UnqPtr<Node>& ParentOfMostLeftRightOf(UnqPtr<Node>& after) {  // нужно при удалении
        // родитель самого левого из правого ребёнка и его потомков
        if (after->right.isNull()) throw std::invalid_argument("No right child");
        if (after->right->left.isNull()) return after;
        UnqPtr<Node>* address = &(after->right);
        while (!((*address)->left->left.isNull())) {  // у левого ребёнка есть левый ребёнок
            address = &((*address)->left);
        }
        return *address;
    }

    int CountAfter(UnqPtr<Node>& node) {
        int left = node->left.isNull() ? 0 : CountAfter(node->left);
        int right = node->right.isNull() ? 0 : CountAfter(node->right);
        return left + right + 1;
    }

    void Traverse(UnqPtr<Node>& node, void (*func)(T), const char* order) {
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

    void ToStringPair(UnqPtr<Node>& node, std::ostringstream& textOut, std::string (*toText)(T)) {
        // Формат: "{parent : child}, "
        if (node->left) {
            textOut << "{" + toText(node->val) + " : " + toText(node->left->val) << "}, ";
            ToStringPair(node->left, textOut, toText);
        }
        if (node->right) {
            textOut << "{" + toText(node->val) + ": " + toText(node->right->val) << "}, ";
            ToStringPair(node->right, textOut, toText);
        }
    }

    bool IsSubTreeNode(UnqPtr<Node>& node, UnqPtr<Node>& lookFor) {
        if (cmp(node->val, lookFor->val != 0)) return false;  // вершины не совпали
        if (lookFor->left.isNull()) {
            if (!(node->left.isNull())) return false;
        } else {
            if (node->left.isNull()) return false;
            if (!IsSubTreeNode(node->left, lookFor->left)) return false;
        }
        if (lookFor->right.isNull()) {
            if (!(node->right.isNull())) return false;
        } else {
            if (node->right.isNull()) return false;
            if (!IsSubTreeNode(node->right, lookFor->right)) return false;
        }
        return true;
    }
public:
    BinaryTree(int (*compare)(T, T)): head(UnqPtr<Node>()), cmp(compare) {}

    ~BinaryTree() {
        head = UnqPtr<Node>();
    }

    void Insert(T item) {
        if (head.isNull()) {
            head = make_unq({item, nullptr, nullptr});
            return;
        }
        UnqPtr<Node>* parent = &head;
        bool finished = false;
        int compared;
        while (finished) {
            compared = cmp(item, (*parent)->val);
            if (compared == 0) return;  // повторяющееся значение
            if (compared == -1) {
                finished = (*parent)->left.isNull();
                if (finished) {
                    (*parent)->left = make_unq({item, nullptr, nullptr});
                } else {
                    parent = &((*parent)->left);
                }
            } else {
                finished = parent->right.isNull();
                if (finished) {
                    (*parent)->right = make_unq({item, nullptr, nullptr});
                } else {
                    parent = &((*parent)->right);
                }
            }
        }
    }

    bool Search(T item) {
        return !(SearchNode(item, head).isNull());
    }

    void Delete(T item) {
        UnqPtr<Node>& node = SearchNode(item, head);
        if (node.isNull()) return;  // значения нет
        if (node->right.isNull()) {
            node = node->left;
        } else {
            UnqPtr<Node>& parentOfNewRoot = ParentOfMostLeftRightOf(node);
            if (cmp(parentOfNewRoot->val, node->val) == 0) {  // это и есть node
                node->val = node->right->val;
                node->right = node->right->right;
            } else {
                node->val = parentOfNewRoot->left->val;
                parentOfNewRoot->left = parentOfNewRoot->left->right;
            }
        }
    }

    int GetCount() {
        return head.isNull() ? 0 : CountAfter(head);
    }

    BinaryTree<T> Map(T (*func)(T), const char* order) {
        BinaryTree<T> result = BinaryTree<T>(cmp);
        auto append = [&](T val) {result.Insert(func(val));};
        Traverse(head, append, order);
        return result;
    }

    template <class U>
    BinaryTree<U> Map(U (*func)(T), int (*cmpU(U, U)), const char* order) {
        BinaryTree<U> result = BinaryTree<T>(cmpU);
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

    template <class U>
    U Reduce(U (*func)(U, T), U initial, const char* order) {
        auto calculate = [&](T val) {initial = func(initial, val);};
        Traverse(calculate, order);
        return initial;
    }

    std::string ToString(std::string (*toText)(T), const char* order) {
        std::string text;
        std::ostringstream textOut(text);
        auto output = [&](T val) {textOut << toText(val);};
        Traverse(head, output, order);
        return text;
    }

    std::string ToStringInPairs(std::string (*toText)(T)) {
        std::string text;
        std::ostringstream textOut(text);
        ToStringPair(head, textOut, toText);
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
            if (head.isNull()) {
                val = fromText(buf);
                Insert(val);
            }
            textIn >> buf;
            if (buf != ":") throw std::invalid_argument("Incorrect string format");
            if (!(textIn >> buf)) throw std::invalid_argument("Incorrect string format");  // нет ребёнка в паре
            bufLength = buf.size();
            if (bufLength < 3) throw std::invalid_argument("Incorrect string format");  // нет },
            if (buf[bufLength - 2] != '}' || buf[bufLength - 1] != ',') {
                throw std::invalid_argument("Incorrect string format");
            }
            buf.erase(bufLength - 2, bufLength - 1);
            val = fromText(buf);
            Insert(val);
        }
    }

    BinaryTree<T> SubTree(T root) {
        UnqPtr<Node>& rootNode = SearchNode(root, head);
        if (rootNode.isNull()) throw std::invalid_argument("No root with given value");
        BinaryTree<T> result = BinaryTree<T>(cmp);
        auto append = [&](T val) {result.Insert(val);};
        Traverse(append, "LCR");  // обход неважен
        return result;
    }

    bool IsSubTree(BinaryTree<T>& subtree) {
        UnqPtr<Node>& rootNode = SearchNode(subtree.head->val, head);
        if (rootNode.isNull()) return false;
        return IsSubTreeNode(rootNode, subtree.head);
    }
};

#endif // BINARYTREE_H
