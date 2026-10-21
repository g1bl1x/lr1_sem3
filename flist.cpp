#include "flist.h"

using namespace std;

bool flist_push(ForwardList& l, string mode, string val, string target) {
    NodeF* n = new NodeF{val, nullptr};
    if (mode == "head" || !l.head) { n->next = l.head; l.head = n; return true; }
    NodeF* curr = l.head;
    if (mode == "tail") {
        while (curr->next) curr = curr->next;
        curr->next = n; return true;
    }
    if (mode == "before" && l.head->val == target) { n->next = l.head; l.head = n; return true; }
    while (curr) {
        if (mode == "before" && curr->next && curr->next->val == target) {
            n->next = curr->next; curr->next = n; return true;
        }
        if (mode == "after" && curr->val == target) {
            n->next = curr->next; curr->next = n; return true;
        }
        curr = curr->next;
    }
    delete n; return false;
}

string flist_pop(ForwardList& l, string mode, string target) {
    if (!l.head) return "Empty";
    NodeF* temp = nullptr; string res = "OK";
    if (mode == "head") {
        temp = l.head; res = temp->val; l.head = l.head->next; delete temp; return res;
    }
    if (mode == "tail") {
        if (!l.head->next) { temp = l.head; res = temp->val; l.head = nullptr; delete temp; return res; }
        NodeF* curr = l.head;
        while (curr->next->next) curr = curr->next;
        temp = curr->next; res = temp->val; curr->next = nullptr; delete temp; return res;
    }
    if (mode == "value") {
        if (l.head->val == target) { temp = l.head; l.head = l.head->next; delete temp; return "OK"; }
        NodeF* curr = l.head;
        while (curr->next) {
            if (curr->next->val == target) { temp = curr->next; curr->next = curr->next->next; delete temp; return "OK"; }
            curr = curr->next;
        }
    }
    if (mode == "before") {
        if (l.head->val == target) return "FALSE"; 
        if (l.head->next && l.head->next->val == target) { 
            temp = l.head; l.head = l.head->next; delete temp; return "OK";
        }
        NodeF* curr = l.head;
        while (curr->next && curr->next->next) {
            if (curr->next->next->val == target) {
                temp = curr->next; curr->next = curr->next->next; delete temp; return "OK";
            }
            curr = curr->next;
        }
    }

    if (mode == "after") {
        NodeF* curr = l.head;
        while (curr) {
            if (curr->val == target) {
                if (!curr->next) return "FALSE"; 
                temp = curr->next; curr->next = curr->next->next; delete temp; return "OK";
            }
            curr = curr->next;
        }
    }
    return "NOT_FOUND";
}

void flist_clear(ForwardList& l) { while (l.head) { NodeF* t = l.head; l.head = l.head->next; delete t; } }
