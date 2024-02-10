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

    void select();
    
    virtual std::type_index type() { return typeid(Node); }

    std::function<void(Node*)> dtor = nullptr;

    auto begin() { return childrens.begin(); }
    auto end() { return childrens.end(); }

    };

template <typename T>
struct Ptr : Node { 
    
    static inline std::unordered_map<std::type_index, std::function<void(Node*,T*)>> oncreate_cbs;
    static inline std::unordered_map<std::type_index, std::function<void(Node*,T*)>> editor_cbs;
    static inline std::unordered_map<std::type_index, std::function<Node*(Node*,Node*)>> onadd_cbs;

    T* ptr; 

    T* get() { return ptr; }

    virtual ~Ptr() { }

    Ptr(void* ptr) 
        : Node((isNode() ? "((Node*)ptr)->name" : boost::typeindex::type_id_with_cvr<T>().pretty_name())), ptr((T*)ptr) { 

            if(oncreate_cbs.find(typeid(T)) != oncreate_cbs.end()) { oncreate_cbs[typeid(T)](this,this->ptr); }

            color = {100,100,100,100};
        
    } 
    
    std::type_index type() override { return typeid(T); }

    void editor() override { if(editor_cbs.size() && editor_cbs.find(typeid(T)) != editor_cbs.end()) editor_cbs[typeid(T)](this,this->ptr); }




    operator T*() { return ptr; }

    static void oncreate(std::function<void(Node*,T*)> cb) { oncreate_cbs[typeid(T)] = cb;  }
    static void editor(std::function<void(Node*,T*)> cb) { editor_cbs[typeid(T)] = cb; }
    template <typename U>
    static void onadd(std::function<Node*(Node*,Node*)> cb) { onadd_cbs[typeid(U)] = cb;  }

    Node* add(Node* node) override {

        if (onadd_cbs.size()) {

            if (onadd_cbs.find(node->type()) != onadd_cbs.end()) {

                return onadd_cbs[node->type()](this,node);

            }

            return nullptr;

        }

        return Node::add(node);

    }


private:
    bool isNode() { return std::is_base_of<Node, T>::value; } 
};


template <typename T>
struct Ownr : Ptr<T> {

    template <typename... Args>
    Ownr(Args&&... args) : Ptr<T>(new T(std::forward<Args>(args)...)) {  }
    
    virtual ~Ownr() { delete Ptr<T>::ptr; }
    
    template <typename U, typename... Args>
    Ownr<U>* add(Args&&... args) { 
    
        Ownr<U>* x = new Ownr<U>(std::forward<Args>(args)...);

        if (!Ptr<T>::add(x)) { delete x; return nullptr; }

        return x;

    }

};