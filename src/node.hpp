#pragma once

#include <cstring>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <functional>
#include "log.hpp"

#include <boost/type_index.hpp>

#include "glm/glm.hpp"

struct File;

struct AnyNode;

struct Node {

    template <typename T>
    static inline std::function<void(Node*,T*)> oncreate_cb = nullptr;
    template <typename T>
    static inline std::function<void(Node*,T*)> onrun_cb = nullptr;
    template <typename T>
    static inline std::function<void(Node*,T*)> editor_cb = nullptr;
    template <typename T>
    static inline std::unordered_map<std::type_index, std::function<Node*(Node*,Node*)>> onadd_cb;

    template <typename T>
    static void onrun(std::function<void(Node*,T*)> cb) { onrun_cb<T> = cb;  }
    template <typename T>
    static void oncreate(std::function<void(Node*,T*)> cb) { oncreate_cb<T> = cb;  }
    template <typename T>
    static void editor(std::function<void(Node*,T*)> cb) { editor_cb<T> = cb; }
    template <typename T>
    static void editor() { editor_cb<T>(); }
    template <typename T>
    void onadd(std::function<Node*(Node*,Node*)> cb) { onadd_cb<T>[type()] = cb;  }

    std::string name;

    glm::vec4 color;

    Node* parent_node = nullptr;
 
    std::vector<Node*> childrens;

    bool active = true, locked = false, loaded = false;

    Node(std::string name = "node", glm::vec4 color = {255,255,255,255});

    virtual ~Node();

    void parent(Node* parent_node);
    
    Node* parent();

    virtual Node* add(Node *node);

    void remove(Node *child);

    uint32_t index();

    void up();

    void down();

    virtual void editor() {}

    virtual void run(); // need to be virtual ?
    
    virtual void update();  // need ?
    
    virtual void runCB();  // need ?

    std::function<void(Node*)> dtor = nullptr; // useless ?

    void import(std::string path); // useless ?

    virtual void import(File* file) {} // useless ?

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

    auto begin() { return childrens.begin(); }

    auto end() { return childrens.end(); }

};

struct Any {};

template <typename T>
struct TypedNode : Node { 

    T* ptr; 

    T* get() { return ptr; }
    
    std::type_index type() override { return typeid(T); }

    void* ptr_untyped() override { return ptr; }

    operator T*() { return ptr; }
    
    TypedNode(void* ptr) : Node((isNode() ? ((Node*)ptr)->name : boost::typeindex::type_id_with_cvr<T>().pretty_name())), ptr((T*)ptr) {

            if(oncreate_cb<T>) { oncreate_cb<T>(this,this->ptr); }

            color = {100,100,100,100};

     }

    void run() override {

        Node::run();

        if(onrun_cb<T>) { onrun_cb<T>(this,this->ptr); }

    }
    Node* add(Node* node) override {

        if (onadd_cb<T>.size()) {

            if (onadd_cb<T>.find(node->type()) != onadd_cb<T>.end()) {

                return onadd_cb<T>[node->type()](this,node);

            }

            return nullptr;

        }

        return Node::add(node);

    }

    void editor() override { if(editor_cb<T>) editor_cb<T>(this,this->ptr); }

    template <typename U>
    static void onadd(std::function<Node*(Node*,Node*)> cb) { onadd_cb<T>[typeid(U)] = cb;  }

private:

    bool isNode() { return std::is_base_of<Node, T>::value; } 

};

struct AnyNode : TypedNode<Any> { };

template <typename T>
struct Ptr : TypedNode<T> { 

    virtual ~Ptr() { }

    Ptr(void* ptr) : TypedNode<T>(ptr) { } 

    AnyNode* addPtr(Node* node) { return (AnyNode*)TypedNode<T>::add(node); }

    template <typename U>
    Ptr<U>* addPtr(U* ptr) { 

        auto x = new Ptr<U>(ptr);

        Ptr<T>::add(x);

        return x;

    }
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