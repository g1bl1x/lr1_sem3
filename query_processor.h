#pragma once
#include "database.h"
#include <string>

std::string execute_query(Database& db, const std::string& query_str);
void file_io(Database& db, std::string filename, bool save);
