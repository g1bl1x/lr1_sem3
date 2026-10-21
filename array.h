#pragma once
#include <string>

struct Array {
    std::string* data = nullptr;
    size_t size = 0;
    size_t capacity = 0;
};

void arr_push(Array& arr, const std::string& val);
bool arr_push_at(Array& arr, size_t idx, const std::string& val);
std::string arr_get(Array& arr, size_t idx, bool& ok);
bool arr_del(Array& arr, size_t idx);
std::string arr_print(const Array& arr);
void arr_clear(Array& arr);
