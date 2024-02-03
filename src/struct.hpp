#pragma once

#include <string>
#include <vector>

struct Component;
struct Struct { 
    
    std::string name;
    std::vector<Component*> comps; 
    size_t size = 0; 

    Struct(std::string name = "struct", std::vector<std::string> components = {});

    void addComp(std::vector<std::string> components);
    
};