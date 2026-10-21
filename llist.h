#pragma once
#include <string>

struct NodeL {
    std::string val;
    NodeL* next = nullptr;
    NodeL* prev = nullptr;
};

struct List {
    NodeL* head = nullptr;
    NodeL* tail = nullptr;
};

bool list_push(List& l, std::string mode, std::string val, std::string target = "");
std::string list_pop(List& l, std::string mode, std::string target = "");
void list_clear(List& l);
