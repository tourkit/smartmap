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

    Instance(Member* buff = nullptr, int eq_id = 0, uint32_t offset = 0);

    Instance(std::string stl_name);

    Instance(const Instance& other);

    struct MemberQ {

        Member* m;
        int eq = 0;
    
    };
    std::vector<MemberQ> stl;

    std::vector<Remap*> remaps;

    uint32_t offset = 0;

    void remap(Instance& inst);

    void remap(std::string stl_name);

    std::string stl_name(std::string separator = "::");

    Member* buff();

    Member* def();

    Instance child(std::string name);

    Instance child(int id);

    Instance parent();

    Instance find(std::string stl_name);

    Instance find(std::vector<Member*> stl);

    Instance& track();

    void calcOffset();

    char* data();
    uint32_t size();

    Instance eq(int id);

    template <typename T>
    T get() { return *((T*)(data())); }

    template <typename T, int U>
    std::array<T,U> get() { return get<std::array<T,U>>(); }

    template <typename T>
    Instance& set(T val) { return set<T>(&val); }

    template <typename T, int U>
    Instance& set(std::array<T,U> val) { return set<std::array<T,U>>(&val); }

    template <typename T, int U, typename... Args>
    Instance& set(Args&&... args) { return set<std::array<T,U>>({std::forward<Args>(args)...}); }

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

    void print();

// private:


    uint32_t eq_id = 0;

};


