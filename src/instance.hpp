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

    struct MemberQ { Member* m; int eq = 0; };
    std::vector<MemberQ> stl;

    uint32_t offset = 0;

    std::string stl_name();

    void loc(int id, int eq = 0) ;

    void loc(Member* m, int eq = 0);

    void loc(std::string stl_name);

    Instance(std::string stl_name);

    Instance(Member& m);

    Instance& track();

    Instance(const Instance& other);

    int size();

    char* data();

    template <typename T>
    T get() { return *(T*) data(); }

    template <typename T, int U>
    std::array<T,U> get() { return get<std::array<T,U>>(); }

    void set(void* ptr, size_t size);

    void calcOffset();

    void setDefault(Member* m, int offset);

};

inline bool operator==(const Instance::MemberQ& this_, const Instance::MemberQ& other){ return (this_.m == other.m && this_.eq == other.eq); }
