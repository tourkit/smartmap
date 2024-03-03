#pragma once

#include <string>
#include <vector>
#include <unordered_set>

struct Component;

static int nextFactor(int x, int factor = 4) { return ((int)(x/(float)factor)+1)*factor; }

struct Struct { 

    static inline std::unordered_set<Struct*> pool;
    
    std::string name;
    std::vector<Component*> comps; 
    size_t size_v = 0; 

    Struct(const char* name = "struct", std::vector<std::string> components = {});

    ~Struct();

    void addComp(std::string component);

    size_t size();
    size_t diff();
    
};