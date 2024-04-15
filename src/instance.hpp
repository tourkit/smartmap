#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>
#include <map>

#include "buffer.hpp"

struct Buffer;
struct Member;

struct Instance {

    Buffer* buff;
    uint32_t offset;
    Member* member = nullptr;

    Instance operator[](std::string name);
    Instance operator[](int id);

    bool exist();

    char* data() { return buff->data.data()+offset; }
    uint32_t size() { return member->footprint_all(); }

    Instance eq(int id);

    template <typename T>
    T get() { return *((T*)(data())); }

    template <typename T>
    Instance& set(T val) {

        PLOGV << member->name() ;

        memcpy(data(), &val, sizeof(T));

        return *this;

    }

    void each(std::function<void(Instance)> cb) { for (int i = 0; i < member->quantity(); i++) cb((*this)[i]); }

    Instance& set(void* ptr, size_t size) {

        memcpy(data(), ptr, size);

        return *this;

    }

    void setDefault(Member* toset = nullptr, int offset = 0);

    Instance push(void* ptr = nullptr, size_t size = 0);

// private:


    uint32_t eq_id = 0;

};
