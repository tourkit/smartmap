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

struct Node;

struct Editor  {

    template <typename U>
    static inline std::function<void(Node*,U*)> cb = nullptr;

    template <typename U>
    static void set(std::function<void(Node*,U*)> cb) { Editor::cb<U> = cb; };

};

struct UntypedNode {

    std::string name;

    glm::vec4 color;

    Node* parent_node = nullptr;
 
    std::vector<Node*> childrens;

    bool active = true, locked = false, loaded = false;

    UntypedNode(std::string name = "node", glm::vec4 color = {255,255,255,255});

    virtual ~UntypedNode();

    void parent(Node* parent_node);
    
    Node* parent();

    virtual Node* add(void *node);

    void remove(Node *child);

    uint32_t index();

    void up();

    void down();

    virtual void editor() { PLOGD << "KOKO";}

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

            if (((UntypedNode*)c)->is_a<V>()) {

                fx(c);

            }

        }       
        
    }
    
    virtual std::type_index type() { return typeid(*this); }
    
    virtual void* ptr_untyped() { return this; }

    auto begin() { return childrens.begin(); }

    auto end() { return childrens.end(); }

    Node* node();

    template <typename U>
    static inline std::function<void(Node*,U*)> oncreate_cb = nullptr;
    template <typename U>
    static inline std::function<void(Node*,U*)> onrun_cb = nullptr;

};

struct Any {};

template <typename T>
struct TypedNode : UntypedNode { 

    template <typename U>
    static inline std::unordered_map<std::type_index, std::function<Node*(Node*,Node*)>> onadd_cb;

    template <typename U>
    static void onadd(std::function<Node*(Node*,Node*)> cb) { onadd_cb<T>[typeid(U)] = cb;  }


    T* ptr; 

    bool owned;
    
    T* get() { return ptr; }
    
    std::type_index type() override { return typeid(T); }

    void* ptr_untyped() override { return ptr; }

    operator T*() { return ptr; }
    
    TypedNode(void* ptr, bool owned = false) : UntypedNode((isNode()? ((UntypedNode*)ptr)->name : boost::typeindex::type_id_with_cvr<T>().pretty_name())), ptr((T*)ptr), owned(owned) {

            if(oncreate_cb<T>) { oncreate_cb<T>((Node*)this,this->ptr); }

            color = {100,100,100,100};

     }

    void run() override {

        UntypedNode::run();

        if(onrun_cb<T>) { onrun_cb<T>((Node*)this,this->ptr); }

    }

    Node* add(void *node_v) override { 
        
        auto node = (TypedNode<Any>*)node_v;

        if (onadd_cb<T>.size()) {

            if (onadd_cb<T>.find(node->type()) != onadd_cb<T>.end()) {

                return (Node*)onadd_cb<T>[node->type()]((Node*)this,(Node*)node);

            }

            return nullptr;

        }

        return UntypedNode::add(node);

    }

    template <typename U>
    Node* addPtr(U* ptr, bool owned = false) { return UntypedNode::add(new TypedNode<U>(ptr, owned)); }

    template <typename U, typename... Args>
    Node* addOwnr(Args&&... args) {

        auto ptr = new U(std::forward<Args>(args)...);
    
        auto* NEW_U = addPtr(ptr,true);

        if (!NEW_U) { delete ptr; return nullptr; }

        return NEW_U;

    }


    ~TypedNode() override { 
        
        if (owned) { 

            delete ptr; 

        } 

    }

    void editor() override { 
        
        if(Editor::cb<T>) {
            Editor::cb<T>(node(),this->ptr); 
        }
    }

private:

    bool isNode() { return std::is_base_of<UntypedNode, T>::value; } 

};

struct Node : TypedNode<Any> { 

    Node(std::string name = "any") : TypedNode<Any>(this) { this->name = name; } 

    template <typename U>
    static void onrun(std::function<void(Node*,U*)> cb) { onrun_cb<U> = cb;  }
    template <typename U>
    static void oncreate(std::function<void(Node*,U*)> cb) { oncreate_cb<U> = cb;  }

    template <typename U>
    static void editor(std::function<void(Node*,U*)> cb) { Editor::cb<U> = cb; }

    void editor() override {  PLOGD << "NIK"; }

};


template <typename T>
struct Ownr : TypedNode<T> {  

    template <typename... Args>
    Ownr(Args&&... args) : TypedNode<T>(new T(std::forward<Args>(args)...), true) { } 
    void editor() override {  PLOGD << "ooooo"; }

};

template <typename T>
struct Ptr : TypedNode<T> { Ptr(T *ptr)  : TypedNode<T>(ptr)  { }
    void editor() override {  PLOGD << "pppp"; } };