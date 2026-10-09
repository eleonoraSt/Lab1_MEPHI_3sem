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
    BinaryTree(int (*compare)(T, T), const char* traversal): head(nullptr), cmp(compare) {}

    ~BinaryTree() {
        if (head) DeleteNode(head);
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
