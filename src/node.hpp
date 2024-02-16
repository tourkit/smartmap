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
struct NodesList;

struct Editor  {

    template <typename U>
    static inline std::function<void(Node*,U*)> cb = nullptr;

    template <typename U>
    static void set(std::function<void(Node*,U*)> cb) { Editor::cb<U> = cb; };

};

struct UntypedNode {

    static inline std::unordered_map<std::type_index,std::unordered_map<std::type_index, std::function<Node*(Node*,Node*)>>> onadd_cb;

    std::string name;

    glm::vec4 color;

    Node* parent_node = nullptr;
    Node* refering = nullptr;
 
    std::vector<Node*> childrens;

    bool active = true, locked = false, loaded = false;

    UntypedNode(std::string name = "node", glm::vec4 color = {255,255,255,255});

    virtual ~UntypedNode();

    void parent(Node* parent_node);
    
    Node* parent();

    NodesList* findRefs(Node* of);

    virtual Node* add(void *node);

    void remove(Node *child);

    uint32_t index();

    bool is_typed = false;

    void up();

    void down();

    virtual void editor() { PLOGD << "KOKO";}

    virtual void run(); 
    
    virtual void update();  


    void runCB(std::function<void(Node*)> cb = nullptr);


    std::function<void(Node*)> dtor = nullptr; // useless ?

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

struct NodesList : UntypedNode {

    std::vector<UntypedNode*> list;

    NodesList (std::vector<UntypedNode*> list = {}) : list(list) {}

};

struct Any {};
struct Passing {};

template <typename T>
struct TypedNode : UntypedNode { 

    T* ptr; 

    bool owned;

    std::type_index stored_type;

    T* get() { return ptr; }
    
    template <typename U>
    U* get() { return (U*)ptr; }
    
    std::type_index type() override { return stored_type; }

    void* ptr_untyped() override { return ptr; }

    operator T*() { return ptr; }
    
    TypedNode(void* ptr, bool owned = false) : 

        UntypedNode((isNode()? ((UntypedNode*)ptr)->name : boost::typeindex::type_id_with_cvr<T>().pretty_name())), 
        ptr((T*)ptr), owned(owned), stored_type(typeid(*this->ptr)) {
    
    // TypedNode(void* ptr, bool owned = false, std::type_index stored_type = typeid(Passing)) : 

    //     UntypedNode((isNode()? ((UntypedNode*)ptr)->name : boost::typeindex::type_id_with_cvr<T>().pretty_name())), 
    //     ptr((T*)ptr), owned(owned), stored_type(stored_type) {

            if (stored_type == typeid(Passing)) stored_type = typeid(*this->ptr);

            if(oncreate_cb<T>) { oncreate_cb<T>((Node*)this,this->ptr); }

            color = {100,100,100,100};

     }

    void run() override {

        UntypedNode::run();

        if(onrun_cb<T>) { onrun_cb<T>((Node*)this,this->ptr); }

    }

    Node* add(void *node_v) override { 
        
        auto node = (TypedNode<Any>*)node_v;

        if (onadd_cb[type()].size()) {

            if (onadd_cb[type()].find(node->type()) != onadd_cb[type()].end()) {

                node = onadd_cb[type()][node->type()](this->node(),node->node());

                if (node->node() == this->node()) return nullptr;

            }

        }

        return UntypedNode::add(node);

    }

    TypedNode(TypedNode<Any>* other) : TypedNode<Any>(other->ptr) { stored_type = other->type(); }

    Node* addPtr(void* node_v) {

        auto node = (TypedNode<Any>*) node_v;

        return add(node); 
         
    }

    template <typename U>
    Node* addPtr(void* ptr) { return TypedNode<T>::add(new TypedNode<U>((U*)ptr)); }

    template <typename U, typename... Args>
    Node* addOwnr(Args&&... args) {

        auto ptr = new U(std::forward<Args>(args)...);
    
        auto* NEW_U = TypedNode<T>::add(new TypedNode<U>(ptr, true));

        if (!NEW_U) { delete ptr; return nullptr; }

        return NEW_U;

    }

    ~TypedNode() override { 
        
        if (owned) { 

            delete ptr; 

        } 

    }

    void editor() override { if(Editor::cb<T>) Editor::cb<T>(node(),this->ptr); }

private:

    bool isNode() { return std::is_base_of<UntypedNode, T>::value; } 

};

struct Node : TypedNode<Any> { 

    Node(std::string name = "any") : TypedNode<Any>(this) { this->name = name; } 

    template <typename U>
    static void editor(std::function<void(Node*,U*)> cb) { Editor::cb<U> = cb; }

    void editor() override {  }

};


template <typename T>
struct Ownr : TypedNode<T> {  

    template <typename... Args>
    Ownr(Args&&... args) : TypedNode<T>(new T(std::forward<Args>(args)...), true) { } 

};



template <typename T>
struct Ptr : TypedNode<T> { Ptr(T *ptr)  : TypedNode<T>(ptr)  { } };

template <typename T>
struct NODE : TypedNode<T> { 
    
    NODE(T *ptr)  : TypedNode<T>(ptr)  { } 

    template <typename U>
    static void onadd(std::function<Node*(Node*,Node*)> cb) { UntypedNode::onadd_cb[typeid(T)][typeid(U)] = cb; }
    
    static void oncreate(std::function<void(Node*,T*)> cb) { UntypedNode::oncreate_cb<T> = cb;  }
    static void onrun(std::function<void(Node*,T*)> cb) { UntypedNode::onrun_cb<T> = cb;  }
    
};