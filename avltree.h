#pragma once
#include <string>
#include <vector>

using std::vector;
using std::string;

struct NodeT {
    std::string val;
    int h = 1;
    NodeT* left = nullptr;
    NodeT* right = nullptr;
};

int get_h(NodeT* n);
int get_b(NodeT* n);
void update_h(NodeT* n);
NodeT* rotate(NodeT* p, bool left);
NodeT* balance(NodeT* n);
NodeT* avl_insert(NodeT* n, std::string val);
NodeT* avl_remove(NodeT* n, std::string val);
bool avl_find(NodeT* n, std::string val);
void avl_collect(NodeT* n, vector<string>& res);
void avl_collect_preorder(NodeT* n, std::vector<std::string>& res);
void avl_destroy(NodeT* n);
