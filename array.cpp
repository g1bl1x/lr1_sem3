#include "array.h"

using namespace std;

void arr_resize(Array& arr) {
    arr.capacity = (arr.capacity == 0) ? 4 : arr.capacity * 2;
    string* new_data = new string[arr.capacity];
    for (size_t i = 0; i < arr.size; ++i) {
        new_data[i] = arr.data[i];
    }
    delete[] arr.data;
    arr.data = new_data;
}

void arr_push(Array& arr, const string& val) {
    if (arr.size >= arr.capacity) {
        arr_resize(arr);
    }
    arr.data[arr.size++] = val;
}

bool arr_push_at(Array& arr, size_t idx, const string& val) {
    if (idx > arr.size) return false;
    if (arr.size >= arr.capacity) {
        arr_resize(arr);
    }
    for (size_t i = arr.size; i > idx; --i) {
        arr.data[i] = arr.data[i - 1];
    }
    arr.data[idx] = val;
    arr.size++;
    return true;
}

string arr_get(Array& arr, size_t idx, bool& ok) {
    if (idx >= arr.size) { ok = false; return ""; }
    ok = true;
    return arr.data[idx];
}

bool arr_del(Array& arr, size_t idx) {
    if (idx >= arr.size) return false;
    for (size_t i = idx; i < arr.size - 1; ++i) {
        arr.data[i] = arr.data[i + 1];
    }
    arr.size--;
    return true;
}

string arr_print(const Array& arr) {
    string res = "[";
    for (size_t i = 0; i < arr.size; ++i) {
        res += arr.data[i] + (i == arr.size - 1 ? "" : ", ");
    }
    res += "]";
    return res;
}

void arr_clear(Array& arr) {
    delete[] arr.data;
    arr.data = nullptr;
    arr.size = 0;
    arr.capacity = 0;
}
