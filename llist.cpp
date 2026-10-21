#include "llist.h"

using namespace std;

bool list_push(List& l, string mode, string val, string target) {
    NodeL* n = new NodeL{val, nullptr, nullptr};
    if (!l.head) { l.head = l.tail = n; return true; }
    if (mode == "head") { n->next = l.head; l.head->prev = n; l.head = n; return true; }
    if (mode == "tail") { n->prev = l.tail; l.tail->next = n; l.tail = n; return true; }
    NodeL* curr = l.head;
    while (curr) {
        if (curr->val == target) {
            if (mode == "before") {
                n->next = curr; n->prev = curr->prev;
                if (curr->prev) curr->prev->next = n; else l.head = n;
                curr->prev = n;
            } else {
                n->prev = curr; n->next = curr->next;
                if (curr->next) curr->next->prev = n; else l.tail = n;
                curr->next = n;
            }
            return true;
        }
        curr = curr->next;
    }
    delete n; return false;
}

string list_pop(List& l, string mode, string target) {
    if (!l.head) return "Empty";
    NodeL* temp = nullptr; string res = "OK";
    if (mode == "head") {
        temp = l.head; res = temp->val; l.head = l.head->next;
        if (l.head) l.head->prev = nullptr; else l.tail = nullptr;
        delete temp; return res;
    }
    if (mode == "tail") {
        temp = l.tail; res = temp->val; l.tail = l.tail->prev;
        if (l.tail) l.tail->next = nullptr; else l.head = nullptr;
        delete temp; return res;
    }
    if (mode == "value") {
        NodeL* curr = l.head;
        while (curr) {
            if (curr->val == target) {
                if (curr->prev) curr->prev->next = curr->next; else l.head = curr->next;
                if (curr->next) curr->next->prev = curr->prev; else l.tail = curr->prev;
                delete curr; return "OK";
            }
            curr = curr->next;
        }
    }

    if (mode == "before") {
        NodeL* curr = l.head;
        while (curr) {
            if (curr->val == target) {
                temp = curr->prev;
                if (!temp) return "FALSE"; 
                if (temp->prev) temp->prev->next = curr; else l.head = curr;
                curr->prev = temp->prev;
                delete temp; return "OK";
            }
            curr = curr->next;
        }
    }

    if (mode == "after") {
        NodeL* curr = l.head;
        while (curr) {
            if (curr->val == target) {
                temp = curr->next;
                if (!temp) return "FALSE"; 
                if (temp->next) temp->next->prev = curr; else l.tail = curr;
                curr->next = temp->next;
                delete temp; return "OK";
            }
            curr = curr->next;
        }
    }
    return "NOT_FOUND";
}

void list_clear(List& l) { while (l.head) { NodeL* t = l.head; l.head = l.head->next; delete t; } l.tail = nullptr; }
