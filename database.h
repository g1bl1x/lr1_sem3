#pragma once
#include <string>
#include <vector>
#include <map>
#include "array.h"
#include "flist.h"
#include "llist.h"
#include "avltree.h"
#include "stack.h"
#include "queue.h"

struct Database {
    std::map<std::string, Array> arrays;
    std::map<std::string, Stack> stacks;
    std::map<std::string, Queue> queues;
    std::map<std::string, ForwardList> f_lists;
    std::map<std::string, List> l_lists;
    std::map<std::string, NodeT*> trees;
};

std::vector<std::string> split(const std::string& s);
