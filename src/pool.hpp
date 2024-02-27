#pragma once

#include <unordered_set>
#include <vector>
#include <cstring>

template <typename T>
struct Pool {

    static inline std::unordered_set<T*> list;

    T* ptr;

    Pool(T* ptr) : ptr(ptr) { add(ptr); }

    ~Pool() { remove(ptr); }

    static void add(T* ptr) { list.insert(ptr);  }

    static void remove(T* ptr) { list.erase(ptr);  }


};