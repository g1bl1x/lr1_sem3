#include "query_processor.h"
#include "array.h"
#include "flist.h"
#include "llist.h"
#include "avltree.h"
#include "stack.h"
#include "queue.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

std::vector<std::string> split(const std::string& s) {
    std::vector<std::string> res; string t; istringstream ss(s);
    while (ss >> t) res.push_back(t);
    return res;
}

string execute_query(Database& db, const string& query_str) {
    auto t = split(query_str); if (t.empty()) return "Error";
    string cmd = t[0]; for (char& c : cmd) c = toupper(c);

    if (cmd == "PRINT" && t.size() > 1) {
        string n = t[1], out = "["; vector<string> tmp;
        if (db.arrays.count(n)) { for (size_t i = 0; i < db.arrays[n].size; ++i) tmp.push_back(db.arrays[n].data[i]); }
        else if (db.stacks.count(n)) { NodeStack* c = db.stacks[n].top; while(c){ tmp.push_back(c->val); c=c->next; } }
        else if (db.queues.count(n)) { NodeQueue* c = db.queues[n].head; while(c){ tmp.push_back(c->val); c=c->next; } }
        else if (db.f_lists.count(n)) { NodeF* c = db.f_lists[n].head; while(c){ tmp.push_back(c->val); c=c->next; } }
        else if (db.l_lists.count(n)) { NodeL* c = db.l_lists[n].head; while(c){ tmp.push_back(c->val); c=c->next; } }
        else if (db.trees.count(n)) avl_collect_preorder(db.trees[n], tmp);
        else return "Not found";
        
        for (size_t i = 0; i < tmp.size(); ++i) out += tmp[i] + (i == tmp.size()-1 ? "" : ", ");
        return out + "]";
    }
    if (cmd.rfind("M", 0) == 0 && t.size() > 1) {
        string n = t[1];
        if (cmd == "MPUSH" && t.size() == 3) { arr_push(db.arrays[n], t[2]); return t[2]; }
        if (cmd == "MPUSH" && t.size() == 4) { if (arr_push_at(db.arrays[n], stoi(t[2]), t[3])) return t[3]; return "Index error"; }
        if (cmd == "MGET" && t.size() == 3) { bool ok; string v = arr_get(db.arrays[n], stoi(t[2]), ok); return ok ? v : "Out of bounds"; }
        if (cmd == "MDEL" && t.size() == 3) { return arr_del(db.arrays[n], stoi(t[2])) ? "OK" : "Error"; }
    }
    if ((cmd.rfind("F", 0) == 0 || cmd.rfind("L", 0) == 0) && t.size() > 1) {
        string n = t[1], mode = t[2];
        if (cmd.substr(1) == "PUSH") {
            string val = t[t.size()-1], target = (t.size() == 5) ? t[3] : "";
            bool ok = (cmd.rfind("F", 0) == 0) ? flist_push(db.f_lists[n], mode, val, target) : list_push(db.l_lists[n], mode, val, target);
            return ok ? "OK" : "FALSE";
        }
        if (cmd.substr(1) == "DEL") {
            string tgt = (t.size() == 4) ? t[3] : "";
            return (cmd.rfind("F", 0) == 0) ? flist_pop(db.f_lists[n], mode, tgt) : list_pop(db.l_lists[n], mode, tgt);
        }
        if (cmd.substr(1) == "GET") {
            if (cmd.rfind("F", 0) == 0) { NodeF* c = db.f_lists[n].head; while(c){ if(c->val==t[2]) return "TRUE"; c=c->next; } } 
            else { NodeL* c = db.l_lists[n].head; while(c){ if(c->val==t[2]) return "TRUE"; c=c->next; } }
            return "FALSE";
        }
    }
    if ((cmd.rfind("S", 0) == 0 || cmd.rfind("Q", 0) == 0) && t.size() > 1) {
        string n = t[1];
        if (cmd.substr(1) == "PUSH" && t.size() == 3) { if (cmd.rfind("S", 0) == 0) stack_push(db.stacks[n], t[2]); else queue_push(db.queues[n], t[2]); return t[2]; }
        if (cmd.substr(1) == "POP") {
            bool ok; string v = (cmd.rfind("S", 0) == 0) ? stack_pop(db.stacks[n], ok) : queue_pop(db.queues[n], ok);
            return ok ? v : "Empty";
        }
    }
    if (cmd.rfind("T", 0) == 0 || cmd == "ISMEMBER") {
        string n = (cmd == "ISMEMBER") ? t[1] : t[1];
        string v = (cmd == "ISMEMBER") ? t[2] : t[2];
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
                if (type == "M") arr_push(db.arrays[n], t[i]);
                if (type == "F") flist_push(db.f_lists[n], "tail", t[i]);
                if (type == "L") list_push(db.l_lists[n], "tail", t[i]);
                if (type == "S") stack_push(db.stacks[n], t[i]);
                if (type == "Q") queue_push(db.queues[n], t[i]);
                if (type == "T") db.trees[n] = avl_insert(db.trees[n], t[i]);
            }
        }
    } else {
        ofstream f(filename);
        for (auto& pair : db.arrays) {
            if(pair.second.size == 0) continue;
            f << "M " << pair.first; for (size_t i = 0; i < pair.second.size; ++i) f << " " << pair.second.data[i]; f << "\n";
        }
        for (auto& pair : db.stacks) {
            if(!pair.second.top) continue;
            f << "S " << pair.first;
            vector<string> tmp; NodeStack* c = pair.second.top; while(c){ tmp.push_back(c->val); c=c->next; }
            reverse(tmp.begin(), tmp.end());
            for (auto& val : tmp) f << " " << val; f << "\n";
        }
        for (auto& pair : db.queues) {
            if(!pair.second.head) continue;
            f << "Q " << pair.first; NodeQueue* c = pair.second.head; while(c){ f << " " << c->val; c=c->next; } f << "\n";
        }
        for (auto& p : db.f_lists) { if(!p.second.head) continue; f << "F " << p.first; NodeF* c = p.second.head; while(c){ f << " " << c->val; c=c->next; } f << "\n"; }
        for (auto& p : db.l_lists) { if(!p.second.head) continue; f << "L " << p.first; NodeL* c = p.second.head; while(c){ f << " " << c->val; c=c->next; } f << "\n"; }
        for (auto& p : db.trees) { if(!p.second) continue; f << "T " << p.first; vector<string> tmp; avl_collect(p.second, tmp); for (auto& v : tmp) f << " " << v; f << "\n"; }
    }
}
