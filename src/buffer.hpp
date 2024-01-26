#pragma once

#include "node.hpp"

struct Buffer : Node {

    std::vector<char> data;  
    
    std::function<void()> callback = [](){};
    
    Node* add(Node* node) override;

    void reset();

    void update() override;
    void editor() override;

    void updateBuffer();
    // void changeObjQuantity();

    Buffer(std::string name = "Buffer");

    // ~Buffer();

private:

    int new_offset();
};