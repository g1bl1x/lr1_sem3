#include "avltree.h"
#include <algorithm>

using namespace std;

int get_h(NodeT* n) { return n ? n->h : 0; }
int get_b(NodeT* n) { return n ? get_h(n->left) - get_h(n->right) : 0; }
void update_h(NodeT* n) { if (n) n->h = 1 + max(get_h(n->left), get_h(n->right)); }

NodeT* rotate(NodeT* p, bool left) {
    NodeT* q = left ? p->right : p->left;
    if (left) { p->right = q->left; q->left = p; } 
    else { p->left = q->right; q->right = p; }
    update_h(p); update_h(q);
    return q;
}

NodeT* balance(NodeT* n) {
    update_h(n);
    if (get_b(n) > 1) {
        if (get_b(n->left) < 0) n->left = rotate(n->left, true);
        return rotate(n, false);
    }
    if (get_b(n) < -1) {
        if (get_b(n->right) > 0) n->right = rotate(n->right, false);
        return rotate(n, true);
    }
    return n;
}

NodeT* avl_insert(NodeT* n, string val) {
    if (!n) return new NodeT{val};
    if (val < n->val) n->left = avl_insert(n->left, val);
    else if (val > n->val) n->right = avl_insert(n->right, val);
    return balance(n);
}

NodeT* avl_remove(NodeT* n, string val) {
    if (!n) return nullptr;
    if (val < n->val) n->left = avl_remove(n->left, val);
    else if (val > n->val) n->right = avl_remove(n->right, val);
    else {
        if (!n->left || !n->right) {
            NodeT* temp = n->left ? n->left : n->right;
            delete n; return temp;
        }
        NodeT* min_node = n->right;
        while (min_node->left) min_node = min_node->left;
        n->val = min_node->val;
        n->right = avl_remove(n->right, min_node->val);
    }
    return balance(n);
}

bool avl_find(NodeT* n, string val) {
    if (!n) return false;
    if (n->val == val) return true;
    return val < n->val ? avl_find(n->left, val) : avl_find(n->right, val);
}

void avl_collect(NodeT* n, vector<string>& res) {
    if (!n) return;
    avl_collect(n->left, res); res.push_back(n->val); avl_collect(n->right, res);
}

void avl_collect_preorder(NodeT* n, vector<string>& res) {
    if (!n) return;
    res.push_back(n->val);
    avl_collect_preorder(n->left, res);
    avl_collect_preorder(n->right, res);
}

void avl_destroy(NodeT* n) { if (n) { avl_destroy(n->left); avl_destroy(n->right); delete n; } }
