
#pragma once

#include <cstdint>
#include <cstring>

#include <algorithm>
#include <string_view>
#include <vector>

//ranger par ordre de longeur

//template <typename T>
//struct Pool {

//        std::vector<T*> list;

//};

struct String {

    static inline std::vector<char> data; // could be 2 (shortstr and longtxt)
    static inline std::vector<String*> pool;

    static void defrag() {

        // create new order list

        // create new empty buffer

        // populate 

        // move new to old

    }

    ////////////////////////////////////////////////////

    uint32_t offset;
    uint32_t length = 0;

    String(const char* value = nullptr) {

        pool.push_back(this);
        offset = data.size();

        if (value)
            set(value);

    }

    String(const String& other)  : offset(other.offset), length(other.length) { }

    ~String() {

        resize(0);

        pool.erase(std::remove(pool.begin(), pool.end(), this), pool.end());
        
    }

    bool set(const char* value);

    char* mut();

    auto view() const;

    String string();

private:

    void resize(size_t length);

public:

};
