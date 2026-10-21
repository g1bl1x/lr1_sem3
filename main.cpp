#include "database.h"
#include "array.h"
#include "flist.h"
#include "llist.h"
#include "avltree.h"
#include "stack.h"
#include "queue.h"
#include "query_processor.h"
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char* argv[]) {
    string file_path = "database.data", query_str = "";
    for (int i = 1; i < argc; ++i) {
        if (string(argv[i]) == "--file" && i + 1 < argc) file_path = argv[++i];
        if (string(argv[i]) == "--query" && i + 1 < argc) query_str = argv[++i];
    }
    if (query_str.empty()) return 1;

    Database db;
    file_io(db, file_path, false);
    cout << execute_query(db, query_str) << "\n";
    file_io(db, file_path, true);
    
    for (auto& p : db.arrays) arr_clear(p.second);
    for (auto& p : db.stacks) stack_clear(p.second);
    for (auto& p : db.queues) queue_clear(p.second);
    for (auto& p : db.f_lists) flist_clear(p.second);
    for (auto& p : db.l_lists) list_clear(p.second);
    for (auto& pair : db.trees) avl_destroy(pair.second);
    return 0;
}
