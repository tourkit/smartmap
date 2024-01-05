#pragma once

#include "pch.hpp"

struct Node {
    
    std::string name = "node";

    uint32_t color[4] = {255,255,255,255};

    std::vector<Node*> childrens;

    std::vector<void*> whitelist;

    Node* parent_node = nullptr;

    ~Node() {

        for (auto c : childrens) c->parent(nullptr);

        if (parent_node) parent_node->unchild(this);

    }

    Node *add(Node* node) {

         node->parent(this);

         return node;

    }

    Node* child(std::string name) { 

        for (auto c : childrens) if (c->name == name) return c;

        return add(new Node{name});

    }

    void unchild(Node *child) {  

        auto it = std::find(childrens.begin(), childrens.end(), child);

        if ( std::distance(childrens.begin(), it) < 0 ) return; 

        childrens.erase(it);

    }

    void parent(Node* parent_node) {  
        
        if (this->parent_node == parent_node) return;

        if (this->parent_node) this->parent_node->unchild(this);
        
        this->parent_node = parent_node;

        if (!parent_node) return;

        parent_node->childrens.push_back(this);
    
    }

    uint32_t index() { 

        auto it = std::find(parent_node->childrens.begin(), parent_node->childrens.end(), this);

        return std::distance(parent_node->childrens.begin(), it);

    }

    void up() { 
        
        if (!parent_node) return;

        auto it = std::find(parent_node->childrens.begin(), parent_node->childrens.end(), this);
        int index = std::distance(parent_node->childrens.begin(), it);

        if(index<1) return; 

        parent_node->childrens.erase(it);
        parent_node->childrens.insert(parent_node->childrens.begin() + index - 1, this);
    
    }

    void down() {

        if (!parent_node) return;
        
        auto it = std::find(parent_node->childrens.begin(), parent_node->childrens.end(), this);
        int index = std::distance(parent_node->childrens.begin(), it);  

        if(index > parent_node->childrens.size()-2) return;

        parent_node->childrens.erase(it);
        parent_node->childrens.insert(parent_node->childrens.begin() + index + 1, this); 

    }

// private:

    // virtual void drawPanel() {

    //     StringBuffer sb; 


    // }

};

// struct GroupNode : Node {

//     GroupNode() : Node{"group"} {}

// };