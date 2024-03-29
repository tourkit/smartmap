#pragma once

#include <string>
#include <vector>
#include <unordered_set>

#include "component.hpp"

static int nextFactor(int x, int factor = 4) { return ((int)((x-1)/(float)factor)+1)*factor*!!x; }
struct Struct { 

    static inline std::unordered_set<Struct*> pool;
    
    std::string name;
    std::vector<Component*> comps; 
    size_t size_v = 0; 
    
    Struct(const char* name = "struct", std::vector<std::string> components = {});

    ~Struct();

    void addComp(std::string component);
    
    void removeComp(std::string component);

    size_t size();
    
    bool is_striding = false;
    void stride(bool is_stride);
    bool stride();

    Struct (const Struct& other);

    
    int addSize(int size);
private:
    
};