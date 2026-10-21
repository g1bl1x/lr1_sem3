#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <algorithm>

using namespace std;

struct NodeF {
    string val;
    NodeF* next = nullptr;
};
struct ForwardList {
    NodeF* head = nullptr;
};

struct NodeL {
    string val;
    NodeL* next = nullptr;
    NodeL* prev = nullptr;
};
struct List {
    NodeL* head = nullptr;
    NodeL* tail = nullptr;
};

struct NodeT {
	string val;
	int h = 1;
	NodeT *left = nullptr, *right = nullptr;
};

struct Database {
    map<string, vector<string>> arrays, stacks, queues;
    map<string, ForwardList> f_lists;
    map<string, List> l_lists;
    map<string, NodeT*> trees;
};

vector<string> split(const string& s) {
    vector<string> res; string t; istringstream ss(s);
    while (ss >> t) res.push_back(t);
    return res;
}

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

void avl_destroy(NodeT* n) { if (n) { avl_destroy(n->left); avl_destroy(n->right); delete n; } }

bool flist_push(ForwardList& l, string mode, string val, string target = "") {
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

string flist_pop(ForwardList& l, string mode, string target = "") {
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
        while (curr && curr->next) {
            if (curr->next->val == target) { temp = curr->next; curr->next = curr->next->next; delete temp; return "OK"; }
            curr = curr->next;
        }
    }
    return "NOT_FOUND";
}

bool list_push(List& l, string mode, string val, string target = "") {
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

string list_pop(List& l, string mode, string target = "") {
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
    return "NOT_FOUND";
}

void flist_clear(ForwardList& l) { while (l.head) { NodeF* t = l.head; l.head = l.head->next; delete t; } }
void list_clear(List& l) { while (l.head) { NodeL* t = l.head; l.head = l.head->next; delete t; } l.tail = nullptr; }

string execute_query(Database& db, const string& query_str) {
    auto t = split(query_str); if (t.empty()) return "Error";
    string cmd = t[0]; for (char& c : cmd) c = toupper(c);

    if (cmd == "PRINT" && t.size() > 1) {
        string n = t[1], out = "["; vector<string> tmp;
        if (db.arrays.count(n)) tmp = db.arrays[n];
        else if (db.stacks.count(n)) tmp = db.stacks[n];
        else if (db.queues.count(n)) tmp = db.queues[n];
        else if (db.f_lists.count(n)) { NodeF* c = db.f_lists[n].head; while(c){ tmp.push_back(c->val); c=c->next; } }
        else if (db.l_lists.count(n)) { NodeL* c = db.l_lists[n].head; while(c){ tmp.push_back(c->val); c=c->next; } }
        else if (db.trees.count(n)) avl_collect(db.trees[n], tmp);
        else return "Not found";
        for (size_t i = 0; i < tmp.size(); ++i) out += tmp[i] + (i == tmp.size()-1 ? "" : ", ");
        return out + "]";
    }
    if (cmd[0] == 'M' && t.size() > 1) {
        auto& a = db.arrays[t[1]];
        if (cmd == "MPUSH" && t.size() == 3) { a.push_back(t[2]); return t[2]; }
        if (cmd == "MPUSH" && t.size() == 4) { 
            size_t idx = stoi(t[2]);
            if (idx <= a.size()) { a.insert(a.begin() + idx, t[3]); return t[3]; }
            return "Index out of bounds";
        }
        if (cmd == "MGET" && t.size() == 3) { 
            size_t idx = stoi(t[2]);
            return idx < a.size() ? a[idx] : "Out of bounds"; 
        }
        if (cmd == "MDEL" && t.size() == 3) { 
            size_t idx = stoi(t[2]);
            if (idx < a.size()) { a.erase(a.begin() + idx); return "OK"; }
            return "Index out of bounds";
        }
    }
    if ((cmd[0] == 'F' || cmd[0] == 'L') && t.size() > 1) {
        string n = t[1], mode = t[2];
        if (cmd.substr(1) == "PUSH") {
            string val = t[t.size()-1], target = (t.size() == 5) ? t[3] : "";
            bool ok = (cmd[0] == 'F') ? flist_push(db.f_lists[n], mode, val, target) : list_push(db.l_lists[n], mode, val, target);
            return ok ? "OK" : "FALSE";
        }
        if (cmd.substr(1) == "DEL") {
            return (cmd[0] == 'F') ? flist_pop(db.f_lists[n], mode, (t.size()==4?t[3]:"")) : list_pop(db.l_lists[n], mode, (t.size()==4?t[3]:""));
        }
        if (cmd.substr(1) == "GET" && t.size() == 3) {
            if (cmd[0] == 'F') { NodeF* c = db.f_lists[n].head; while(c){ if(c->val==t[2]) return "TRUE"; c=c->next; } } 
            else { NodeL* c = db.l_lists[n].head; while(c){ if(c->val==t[2]) return "TRUE"; c=c->next; } }
            return "FALSE";
        }
    }
    if ((cmd[0] == 'S' || cmd[0] == 'Q') && t.size() > 1) {
        auto& c = (cmd[0] == 'S') ? db.stacks[t[1]] : db.queues[t[1]];
        if (cmd.substr(1) == "PUSH" && t.size() == 3) { c.push_back(t[2]); return t[2]; }
        if (cmd.substr(1) == "POP") {
            if (c.empty()) return "Empty";
            string v = (cmd[0] == 'S') ? c.back() : c.front();
            c.erase((cmd[0] == 'S') ? c.end() - 1 : c.begin()); return v;
        }
    }
    if (cmd == "TINSERT" || cmd == "TDEL" || cmd == "TGET" || cmd == "ISMEMBER") {
        string n = t[1], v = t[2];
        if (cmd == "TINSERT") { db.trees[n] = avl_insert(db.trees[n], v); return v; }
        if (cmd == "TDEL") { db.trees[n] = avl_remove(db.trees[n], v); return "OK"; }
        if (cmd == "TGET" || cmd == "ISMEMBER") return avl_find(db.trees[n], v) ? "TRUE" : "FALSE";
    }
    return "Unknown command";
}

void file_io(Database& db, string filename, bool save) {
    if (!save) {
        ifstream f(filename); string line;
        while (getline(f, line)) {
            auto t = split(line); if (t.size() < 3) continue;
            string type = t[0], n = t[1];
            for (size_t i = 2; i < t.size(); ++i) {
                if (type == "M") db.arrays[n].push_back(t[i]);
                if (type == "F") flist_push(db.f_lists[n], "tail", t[i]);
                if (type == "L") list_push(db.l_lists[n], "tail", t[i]);
                if (type == "S") db.stacks[n].push_back(t[i]);
                if (type == "Q") db.queues[n].push_back(t[i]);
                if (type == "T") db.trees[n] = avl_insert(db.trees[n], t[i]);
            }
        }
    } else {
        ofstream f(filename);
        auto wr = [&](string p, auto& map) {
            for (auto& pair : map) {
                if(pair.second.empty()) continue;
                f << p << " " << pair.first;
                for (auto& val : pair.second) f << " " << val; f << "\n";
            }
        };

        wr("M", db.arrays); wr("S", db.stacks); wr("Q", db.queues);
        for (auto& p : db.f_lists) { if(!p.second.head) continue; f << "F " << p.first; NodeF* c = p.second.head; while(c){ f << " " << c->val; c=c->next; } f << "\n"; }
        for (auto& p : db.l_lists) { if(!p.second.head) continue; f << "L " << p.first; NodeL* c = p.second.head; while(c){ f << " " << c->val; c=c->next; } f << "\n"; }
        for (auto& p : db.trees) { if(!p.second) continue; f << "T " << p.first; vector<string> tmp; avl_collect(p.second, tmp); for (auto& v : tmp) f << " " << v; f << "\n"; }
    }
}

int main(int argc, char* argv[]) {
    string file_path = "database.data", query_str = "";
    for (int i = 1; i < argc; ++i) {
        if (string(argv[i]) == "--file" && i + 1 < argc) file_path = argv[++i];
        else if (string(argv[i]) == "--query" && i + 1 < argc) query_str = argv[++i];
    }

    if (query_str.empty()) return 1;
    Database db;
    file_io(db, file_path, false);
    cout << execute_query(db, query_str) << "\n";
    file_io(db, file_path, true);
    for (auto& p : db.f_lists) flist_clear(p.second);
    for (auto& p : db.l_lists) list_clear(p.second);
    for (auto& pair : db.trees) avl_destroy(pair.second);
    return 0;
}