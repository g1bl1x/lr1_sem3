#pragma once
#include <string>

using std::string;

struct NodeF {
    std::string val;
    NodeF* next = nullptr;
};

struct ForwardList {
    NodeF* head = nullptr;
};

bool flist_push(ForwardList& l, std::string mode, std::string val, std::string target = "");
std::string flist_pop(ForwardList& l, std::string mode, std::string target = "");
void flist_clear(ForwardList& l);
