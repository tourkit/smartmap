#pragma once

#include <cstring>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <functional>
#include "log.hpp"

#include <boost/type_index.hpp>

struct File;

struct Node {
    
    std::string name;

    std::vector<uint32_t> color;

    Node* parent_node = nullptr;

    std::vector<Node*> childrens;

    bool active = true, locked = false, loaded = false;

    Node(std::string name = "node", std::vector<uint32_t> color = {255,255,255,255});

    virtual ~Node();

    template <typename U>
    U* is_a() { return ((typeid(*this) == typeid(U))? (U*)this : nullptr); }

    void parent(Node* parent_node);
    
    Node* parent();

    virtual Node *add(Node *node);

    void remove(Node *child);

    uint32_t index();

    void up();

    void down();

    virtual void editor() {}

    void import(std::string path);

    virtual void import(File* file) {}

    virtual void run();
    virtual void runCB();
    
    virtual void update();

    std::function<void(Node*)> dtor = nullptr;

    auto begin() { return childrens.begin(); }
    auto end() { return childrens.end(); }

    };

template <typename T>
struct Ptr : Node { 
    
    static inline std::unordered_map<std::type_index, std::function<void(Node*,T*)>> onadd_cbs;
    static inline std::unordered_map<std::type_index, std::function<void(Node*,T*)>> editor_cbs;
    static inline std::unordered_map<std::type_index, std::function<Node*(Node*,Node*)>> whitelist_cbs;

    T* ptr; 

    T* get() { return ptr; }

    virtual ~Ptr() { }

    Ptr(void* ptr) 
        : Node((isNode() ? ((Node*)ptr)->name : boost::typeindex::type_id_with_cvr<T>().pretty_name() + " ptr")), ptr((T*)ptr) { 

            if(onadd_cbs.find(typeid(T)) != onadd_cbs.end()) { onadd_cbs[typeid(T)](this,this->ptr); }

            color = {100,100,100,100};
        
    } 
    
    Node *add(Node* node) override {

        if(whitelist_cbs.size() && whitelist_cbs.find(typeid(*node)) != whitelist_cbs.end()) {

            return whitelist_cbs[typeid(*node)](this,node);

        }

        return Node::add(node);


    }

    void editor() override { if(editor_cbs.size() && editor_cbs.find(typeid(T)) != editor_cbs.end()) editor_cbs[typeid(T)](this,this->ptr); }




    operator T*() { return ptr; }

    static void onadd(std::function<void(Node*,T*)> cb) { onadd_cbs[typeid(T)] = cb;  }
    static void editor(std::function<void(Node*,T*)> cb) { editor_cbs[typeid(T)] = cb; }
    template <typename U>
    static void whitelist(std::function<Node*(Node*,Node*)> cb) { whitelist_cbs[typeid(Ptr<U>)] = cb;  }

private:
    bool isNode() { return std::is_base_of<Node, T>::value; } 
};

struct N {};

template <typename T>
struct NODE : Ptr<T> {

    template <typename... Args>
    NODE(Args&&... args) : Ptr<T>(new T(std::forward<Args>(args)...)) {  }
    
    virtual ~NODE() { delete Ptr<T>::ptr; }
    
    template <typename U, typename... Args>
    NODE<U>* add(Args&&... args) { return (NODE<U>*)Node::add(new NODE<U>(std::forward<Args>(args)...)); }

};