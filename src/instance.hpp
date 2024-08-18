#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>
#include <map>

#include "buffer.hpp"

struct Buffer;
struct Member;
struct Instance;


struct Remap {

    Instance *src, *dst;

    virtual void update();

    int quantity;

    Remap(Instance* src ,Instance* dst, int quantity = 1);


};

struct Instance {

    Buffer* buff;

    std::vector<Member*> stl;
    std::vector<Remap*> remaps;

    void remap(Instance& inst);

    uint32_t offset = -1;

    std::string stl_name(std::string separator = "::");

    Member* def();

    Instance(const Instance& other);

    Instance(std::string stl_name);

    Instance(Buffer* buff, Member* m);

    Instance(Buffer* buff = nullptr, uint32_t offset = 0, std::vector<Member*> stl = {}, int eq_id = 0);

    Instance operator[](std::string name);

    Instance operator[](int id);

    Instance find(Member* m);

    Instance parent();

    bool exist();

    Instance& track();

    char* data();

    uint32_t size();

    Instance eq(int id);

    template <typename T>
    T get() { return *((T*)(data())); }

    template <typename T>
    Instance& set(T val) { return set<T>(&val); }

    template <typename T>
    Instance& set(void* val) {

        // PLOGV << def()->name() ;

        return set(val, sizeof(T));

    }

    Instance& set(void* ptr, size_t size);

    void update();

    void setDefault(Member* toset = nullptr, int offset = 0);

    Instance push(void* ptr = nullptr, size_t size = 0);

    void each(std::function<void(Instance&)> f);

// private:


    uint32_t eq_id = 0;

};


