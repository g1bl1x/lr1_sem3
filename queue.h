#pragma once
#include <string>

struct NodeQueue {
    std::string val;
    NodeQueue* next = nullptr;
};

struct Queue {
    NodeQueue* head = nullptr;
    NodeQueue* tail = nullptr;
};

void queue_push(Queue& q, const std::string& val);
std::string queue_pop(Queue& q, bool& ok);
void queue_clear(Queue& q);
