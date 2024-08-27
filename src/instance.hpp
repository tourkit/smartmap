#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>
#include "utils.hpp"

struct Member;
struct Remap;

struct Instance {

    std::vector<MemberQ> stl;

    uint32_t offset = 0;
    
    std::vector<Remap*> remaps;

    std::string stl_name();

    Instance& loc(int id, int eq = 0) ;

    Instance& loc(Member* m, int eq = 0);

    Instance& loc(std::string stl_name);

    Instance& eq(int id);
    
    int eq();

    Instance(std::string stl_name);

    Instance(Member& m);

    Instance(const Instance& other);

    ~Instance();
    
    int size();
    
    std::string print(bool label = false, Member* m = nullptr, int offset = -1, std::string prefix = "");

    char* data();

    template <typename T>
    T get() { return *(T*) data(); }

    template <typename T, int U>
    std::array<T,U> get() { return get<std::array<T,U>>(); }


    template <typename T>
    void set(T val) { set<T>(&val); }

    template <typename T, int U>
    void set(std::array<T,U> val) { set<std::array<T,U>>(&val); }

    template <typename T, int U, typename... Args>
    void set(Args&&... args) { set<std::array<T,U>>({std::forward<Args>(args)...}); }

    template <typename T>
    void set(void* val) { set(val, sizeof(T)); }

    void set(void* ptr, size_t size);

    void setDefault(Member* m = nullptr, int offset = 0);

    void stlAdd(Member* m, int eq = 0);

    void each(std::function<void(Instance&)> f);
    
    Instance& operator[](int id)  { return loc(id); }

    Instance& operator[](Member* m) { return loc(m); }

    Instance& operator[](std::string stl_name) { return loc(stl_name); }
    
    void pre_change();
    
    void post_change(std::vector<MemberQ> added = {});

};
