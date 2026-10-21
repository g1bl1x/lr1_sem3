#pragma once
#include <string>

struct NodeStack {
    std::string val;
    NodeStack* next = nullptr;
};

struct Stack {
    NodeStack* top = nullptr;
};

void stack_push(Stack& s, const std::string& val);
std::string stack_pop(Stack& s, bool& ok);
void stack_clear(Stack& s);
