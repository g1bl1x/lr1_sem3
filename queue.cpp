#include "queue.h"

using namespace std;

void queue_push(Queue& q, const string& val) {
    NodeQueue* n = new NodeQueue{val, nullptr};
    if (!q.head) {
        q.head = n;
        q.tail = n;
    } else {
        q.tail->next = n;
        q.tail = n;
    }
}

string queue_pop(Queue& q, bool& ok) {
    if (!q.head) {
        ok = false;
        return "";
    }
    ok = true;
    NodeQueue* temp = q.head;
    string v = temp->val;
    q.head = q.head->next;
    if (!q.head) {
        q.tail = nullptr;
    }
    delete temp;
    return v;
}

void queue_clear(Queue& q) {
    while (q.head) {
        NodeQueue* temp = q.head;
        q.head = q.head->next;
        delete temp;
    }
    q.tail = nullptr;
}
