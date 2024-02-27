#pragma once

#include <string>
#include <vector>
#include <unordered_set>

struct Component;

struct Struct { 

    static inline std::unordered_set<Struct*> pool;
    
    std::string name;
    std::vector<Component*> comps; 
    size_t size = 0; 

    Struct(const char* name = "struct", std::vector<std::string> components = {});

    ~Struct();

    void addComp(std::string component);

    size_t getComp(std::string name) {

        return 0;
        
    }
    
};