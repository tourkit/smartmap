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
struct NODE;


struct Node {

    template <typename T>
    static inline std::function<void(Node*,T*)> oncreate_cbs2;
    template <typename T>
    static inline std::function<void(Node*,T*)> editor_cbs2;
    template <typename T>
    static inline std::unordered_map<std::type_index, std::function<Node*(Node*,Node*)>> onadd_cbs2;

    template <typename T>
    static void oncreate(std::function<void(Node*,T*)> cb) { oncreate_cbs2<T> = cb;  }
    template <typename T>
    static void editor(std::function<void(Node*,T*)> cb) { editor_cbs2<T> = cb; }
    template <typename T>
    static void editor() { editor_cbs2<T>(); }
    template <typename T>
    void onadd(std::function<Node*(Node*,Node*)> cb) { onadd_cbs2<T>[type()] = cb;  }

    std::string name;

    std::vector<uint32_t> color;

    Node* parent_node = nullptr;
 
    std::vector<Node*> childrens;

    bool active = true, locked = false, loaded = false;

    Node(std::string name = "node", std::vector<uint32_t> color = {255,255,255,255});

    virtual ~Node();

    void parent(Node* parent_node);
    
    Node* parent();

    virtual Node *add(Node *node);

    virtual NODE* addPtr(Node* node) ;

    void remove(Node *child);

    uint32_t index();

    void up();

    void down();

    virtual void editor() {}

    virtual void run();
    
    virtual void update();

    void select();

    template <typename U>
    U* is_a() { return ((type() == typeid(U))? (U*)ptr_untyped() : nullptr); }

    template <typename V>
    void each(std::function<void(Node*)> fx) { 
        
        for (auto c : childrens) {

            if (c->is_a<V>()) {

                fx(c);

            }

        }       
        
    }
    
    virtual std::type_index type() { return typeid(Node); }
    
    virtual void* ptr_untyped() { return this; }

    std::function<void(Node*)> dtor = nullptr; // useless ?

    auto begin() { return childrens.begin(); }

    auto end() { return childrens.end(); }
    
    virtual void runCB();

    void import(std::string path); // useless ?

    virtual void import(File* file) {} // useless ?

    };

template <typename T>
struct Ptr : Node { 

    T* ptr; 

    T* get() { return ptr; }
    
    std::type_index type() override { return typeid(T); }

    void* ptr_untyped() override { return ptr; }

    operator T*() { return ptr; }
    
    Ptr(void* ptr) : Node((isNode() ? "((Node*)ptr)->name" : boost::typeindex::type_id_with_cvr<T>().pretty_name())), ptr((T*)ptr) {

            if(oncreate_cbs2<T>) { oncreate_cbs2<T>(this,this->ptr); }

            color = {100,100,100,100};

     }

    Node* add(Node* node) override {

        if (onadd_cbs2<T>.size()) {

            if (onadd_cbs2<T>.find(node->type()) != onadd_cbs2<T>.end()) {

                return onadd_cbs2<T>[node->type()](this,node);

            }

            return nullptr;

        }

        return Node::add(node);

    }

    void editor() override { if(editor_cbs2<T>) editor_cbs2<T>(this,this->ptr); }

    template <typename U>
    static void onadd(std::function<Node*(Node*,Node*)> cb) { onadd_cbs2<T>[typeid(U)] = cb;  }

private:

    bool isNode() { return std::is_base_of<Node, T>::value; } 

};

struct ANYNODE {};
struct NODE : Ptr<ANYNODE> { };

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