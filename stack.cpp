#include "stack.h"

using namespace std;

void stack_push(Stack& s, const string& val) {
    NodeStack* n = new NodeStack{val, s.top};
    s.top = n;
}

string stack_pop(Stack& s, bool& ok) {
    if (!s.top) {
        ok = false;
        return "";
    }
    ok = true;
    NodeStack* temp = s.top;
    string v = temp->val;
    s.top = s.top->next;
    delete temp;
    return v;
}

void stack_clear(Stack& s) {
    while (s.top) {
        NodeStack* temp = s.top;
        s.top = s.top->next;
        delete temp;
    }
}
