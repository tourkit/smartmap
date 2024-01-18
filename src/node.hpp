#pragma once

#include "pch.hpp"
#include <boost/type_index.hpp>
struct File;

struct Node {
    
    std::string name;

    std::vector<uint32_t> color;

    Node* parent_node = nullptr;

    std::vector<Node*> childrens;

    bool active = true, locked = false;

    Node(std::string name = "node", std::vector<uint32_t> color = {255,255,255,255});

    virtual ~Node();

    template <typename U>
    bool is_a() { return (typeid(*this) == typeid(U)); }

    void parent(Node* parent_node) ;

    virtual Node *add(Node *node);

    void remove(Node *child);

    uint32_t index();

    void up();

    void down();

    virtual void editor() {}

    void import(std::string path);

    virtual void import(File* file) {}

};

template <typename T>
struct Ptr : Node { 
    
    T* ptr; 
    bool owned;

    virtual ~Ptr() { if (owned) delete ptr; }

    Ptr(void* ptr, bool owned = false) 
        : ptr((T*)ptr), owned(owned) { 
         
        if (isNode()) { name = ((Node*)ptr)->name; }
        else { name = boost::typeindex::type_id_with_cvr<T>().pretty_name(); }
        name += " ptr"; 
        
    } 
    
    Node* add(Node* n) override { return nullptr; }  

    operator T*() { return ptr; }

private:
    bool isNode() { return std::is_base_of<Node, T>::value; } 
};

template <typename T>
struct Ownr : Ptr<T> {

    template <typename... Args>
    Ownr(Args&&... args) : Ptr<T>(new T(std::forward<Args>(args)...), true) { }

};


// struct GroupNode : Node {

//     GroupNode() : Node{"group"} {}

// };

