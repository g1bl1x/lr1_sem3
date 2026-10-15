#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <algorithm>
#include <list>

using namespace std;

// 1. АВЛ-ДЕРЕВО
struct NodeT {
    string val;
    int h = 1;
    NodeT *left = nullptr, *right = nullptr;
};

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

// 2. БАЗА ДАННЫХ И ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
struct Database {
    map<string, vector<string>> arrays, stacks, queues;
    map<string, list<string>> f_lists, l_lists;
    map<string, NodeT*> trees;
};

vector<string> split(const string& s) {
    vector<string> res; string t; istringstream ss(s);
    while (ss >> t) res.push_back(t);
    return res;
}

bool list_push(list<string>& l, string mode, string val, string target = "") {
    if (mode == "head") { l.push_front(val); return true; }
    if (mode == "tail") { l.push_back(val); return true; }
    auto it = find(l.begin(), l.end(), target);
    if (it == l.end()) return false;
    if (mode == "before") l.insert(it, val);
    else l.insert(next(it), val);
    return true;
}

// 3. ОБРАБОТЧИК КОМАНД
string execute_query(Database& db, const string& query_str) {
    auto t = split(query_str); if (t.empty()) return "Error";
    string cmd = t[0]; for (char& c : cmd) c = toupper(c);

    // Глобальная команда PRINT
    if (cmd == "PRINT" && t.size() > 1) {
        string n = t[1], out = "[";
        vector<string> tmp;
        if (db.arrays.count(n)) tmp = db.arrays[n];
        else if (db.stacks.count(n)) tmp = db.stacks[n];
        else if (db.queues.count(n)) tmp = db.queues[n];
        else if (db.f_lists.count(n)) tmp = vector<string>(db.f_lists[n].begin(), db.f_lists[n].end());
        else if (db.l_lists.count(n)) tmp = vector<string>(db.l_lists[n].begin(), db.l_lists[n].end());
        else if (db.trees.count(n)) avl_collect(db.trees[n], tmp);
        else return "Not found";
        
        for (size_t i = 0; i < tmp.size(); ++i) out += tmp[i] + (i == tmp.size()-1 ? "" : ", ");
        return out + "]";
    }

    // Обработка Массивов (M)
    if (cmd[0] == 'M' && t.size() > 1) {
        auto& a = db.arrays[t[1]];
        if (cmd == "MPUSH" && t.size() == 3) { a.push_back(t[2]); return t[2]; }
        if (cmd == "MPUSH" && t.size() == 4) { a.insert(a.begin() + stoi(t[2]), t[3]); return t[3]; }
        if (cmd == "MGET") return stoi(t[2]) < a.size() ? a[stoi(t[2])] : "Out of bounds";
        if (cmd == "MDEL") { a.erase(a.begin() + stoi(t[2])); return "OK"; }
    }
    // Обработка Списков (F и L)
    if ((cmd[0] == 'F' || cmd[0] == 'L') && t.size() > 1) {
        auto& l = (cmd[0] == 'F') ? db.f_lists[t[1]] : db.l_lists[t[1]];
        if (cmd.substr(1) == "PUSH") return list_push(l, t[2], t.size() == 5 ? t[4] : t[3], t.size() == 5 ? t[3] : "") ? "OK" : "FALSE";
        if (cmd.substr(1) == "DEL") {
            if (l.empty()) return "Empty";
            if (t[2] == "head") { string v = l.front(); l.pop_front(); return v; }
            if (t[2] == "tail") { string v = l.back(); l.pop_back(); return v; }
            if (t[2] == "value") { auto it = find(l.begin(), l.end(), t[3]); if (it != l.end()) { l.erase(it); return "OK"; } return "NOT_FOUND"; }
        }
        if (cmd.substr(1) == "GET") return find(l.begin(), l.end(), t[2]) != l.end() ? "TRUE" : "FALSE";
    }
    // Обработка Стека (S) и Очереди (Q)
    if ((cmd[0] == 'S' || cmd[0] == 'Q') && t.size() > 1) {
        auto& c = (cmd[0] == 'S') ? db.stacks[t[1]] : db.queues[t[1]];
        if (cmd.substr(1) == "PUSH") { c.push_back(t[2]); return t[2]; }
        if (cmd.substr(1) == "POP") {
            if (c.empty()) return "Empty";
            string v = (cmd[0] == 'S') ? c.back() : c.front();
            c.erase((cmd[0] == 'S') ? c.end() - 1 : c.begin());
            return v;
        }
    }
    // Обработка АВЛ-Дерева (T или ISMEMBER)
    if (cmd[0] == 'T' || cmd == "ISMEMBER") {
        string n = (cmd == "ISMEMBER") ? t[1] : t[1];
        string v = (cmd == "ISMEMBER") ? t[2] : t[2];
        if (cmd == "TINSERT") { db.trees[n] = avl_insert(db.trees[n], v); return v; }
        if (cmd == "TDEL") { db.trees[n] = avl_remove(db.trees[n], v); return "OK"; }
        if (cmd == "TGET" || cmd == "ISMEMBER") return avl_find(db.trees[n], v) ? "TRUE" : "FALSE";
    }
    return "Unknown command";
}

// ============================================================================
// 4. РАБОТА С ФАЙЛАМИ И MAIN
// ============================================================================
void file_io(Database& db, string filename, bool save) {
    if (!save) {
        ifstream f(filename); string line;
        while (getline(f, line)) {
            auto t = split(line); if (t.size() < 3) continue;
            string type = t[0], n = t[1];
            for (size_t i = 2; i < t.size(); ++i) {
                if (type == "M") db.arrays[n].push_back(t[i]);
                if (type == "F") db.f_lists[n].push_back(t[i]);
                if (type == "L") db.l_lists[n].push_back(t[i]);
                if (type == "S") db.stacks[n].push_back(t[i]);
                if (type == "Q") db.queues[n].push_back(t[i]);
                if (type == "T") db.trees[n] = avl_insert(db.trees[n], t[i]);
            }
        }
    } else {
        ofstream f(filename);
        auto wr = [&](string p, auto& map) {
            for (auto& pair : map) {
                f << p << " " << pair.first;
                for (auto& val : pair.second) f << " " << val;
                f << "\n";
            }
        };
        wr("M", db.arrays); wr("S", db.stacks); wr("Q", db.queues);
        for (auto& p : db.f_lists) { f << "F " << p.first; for (auto& v : p.second) f << " " << v; f << "\n"; }
        for (auto& p : db.l_lists) { f << "L " << p.first; for (auto& v : p.second) f << " " << v; f << "\n"; }
        for (auto& p : db.trees) {
            f << "T " << p.first; vector<string> tmp; avl_collect(p.second, tmp);
            for (auto& v : tmp) f << " " << v; f << "\n";
        }
    }
}

int main(int argc, char* argv[]) {
    string file_path = "database.data", query_str = "";
    for (int i = 1; i < argc; ++i) {
        if (string(argv[i]) == "--file") file_path = argv[++i];
        if (string(argv[i]) == "--query") query_str = argv[++i];
    }
    if (query_str.empty()) return 1;

    Database db;
    file_io(db, file_path, false);
    cout << execute_query(db, query_str) << "\n";
    file_io(db, file_path, true);
    
    for (auto& pair : db.trees) avl_destroy(pair.second);
    return 0;
}
