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

    uint32_t offset = -1;

    std::string stl_name(std::string separator = "::");

    Member* def() { if(stl.size()) return stl.back(); return buff; }

    Instance(Buffer* buff, Member* m);
    Instance(Buffer* buff = nullptr, uint32_t offset = 0, std::vector<Member*> stl = {}, int eq_id = 0);


    Instance operator[](std::string name);

    Instance operator[](int id);

    Instance find(Member* m);

    Instance parent();

    bool exist();

    Instance& track() {

        def()->instances.emplace_back(std::make_shared<Instance>(buff, offset, stl));

        return *def()->instances.back().get();

    }

    char* data() { return buff->data.data()+offset; }

    uint32_t size() { return def()->footprint_all(); }

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
    Instance& set(void* ptr, size_t size) {

        memcpy(data(), ptr, size);

        for (auto &inst : def()->instances) for (auto r : inst.get()->remaps) r->update();

        return *this;

    }

    void setDefault(Member* toset = nullptr, int offset = 0);

    Instance push(void* ptr = nullptr, size_t size = 0);

// private:


    uint32_t eq_id = 0;

};
