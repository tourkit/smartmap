#pragma once

#include "pch.hpp"

struct Node {
    
    std::string name;

    std::vector<uint32_t> color;

    Node* parent_node = nullptr;

    std::vector<Node*> childrens;

    Node(std::string name = "node", std::vector<uint32_t> color = {255,255,255,255});

    ~Node();

    template <typename U>
    bool is_a() { return (typeid(*this) == typeid(U)); }

    void parent(Node* parent_node) ;

    virtual Node *add(Node *node);

    void remove(Node *child);

    uint32_t index();

    void up();

    void down();

    virtual void editor() {}

};

template <typename T>
struct Ptr : Node { T* ptr; Ptr(void* ptr) : ptr((T*)ptr) { name = ((Node*)ptr)->name + "ptr"; } };

// struct GroupNode : Node {

//     GroupNode() : Node{"group"} {}

// };

