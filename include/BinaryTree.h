#ifndef BINARYTREE_H
#define BINARYTREE_H

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

    void deleteNode(Node* node) {
        if (node->left) deleteNode(node->left);
        if (node->right) deleteNode(node->right);
        delete node;
    }
public:
    BinaryTree(int (*compare)(T, T)): head(nullptr), cmp(compare) {}

    ~BinaryTree() {
        if (head) deleteNode(head);
    }

    void Insert(T item) {
        Node node = {item, nullptr, nullptr};
        Node* insertAfter = head;
        while (insertAfter) {
            if (cmp(item, insertAfter->val) == 0) return;
            insertAfter = cmp(item, insertAfter->val) == -1 ? insertAfter->left : insertAfter->right;
        }
        insertAfter = new Node(node);
    }

    bool Search(T item) {
        Node* lookAt = head;
        while (lookAt) {
            if (cmp(item, lookAt->val) == 0) return true;
            lookAt = cmp(item, lookAt->val) == -1 ? lookAt->left : lookAt->right;
        }
        return false;
    }
};

#endif // BINARYTREE_H
