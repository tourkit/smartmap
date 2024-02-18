#pragma once

#include <string>
#include <vector>

struct Component;
struct Struct { 
    
    std::string name;
    std::vector<Component*> comps; 
    size_t size = 0; 

    Struct(const char* name = "struct", std::vector<std::string> components = {});

    void addComp(std::string component);

    size_t getComp(std::string name) {

        return 0;
        
    }
    
};