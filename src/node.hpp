#pragma once

#include <cstring>
#include <vector>
#include <set>
#include <map>
#include <functional>

#include "utils.hpp"


#include "imgui/imgui.h"

struct Node {

    enum Event {

        CREATE,
        CHANGE,
        RUN,
        DELETE

    };

    std::string name_v;

#ifdef ROCH
    std::string TYPE = "Node";
#endif

    ImVec4 color = {1,1,1,1};

    std::vector<Node*> childrens;

    std::vector<Node*> hidden_childrens;

    bool locked = false, loaded = false, hidden = false, open = true, is_active = false;

    static inline std::set<Node*> pool;

    Node(std::string name = "node", ImVec4 color = {1,1,1,1});
    Node(void* ptr, TypeIndex type, bool owned);


    Node(Node* other) : Node(other->void_ptr, other->stored_type, other->owned) {  }

    virtual ~Node();

    void init();

    const std::string& name();

    Node* name(std::string value);

    void editor();

    virtual void update();

    Node* add(void *node);

    void parent(Node* parent_node);

    Node* parent();

    Node* child(std::string  name);
    Node* child(std::vector<std::string> names);

    uint32_t index();

    void addList(std::vector<Node*> *nodes);

    Node* hide();

    Node* close();

    Node* active(bool value);

    Node* select();

    bool remove(Node *child);

    TypeIndex type() { return stored_type; }

    virtual std::string type_name() { return type().pretty_name();  }

    Node* on(Event event, std::function<void(Node*)> cb = nullptr);

    void each_(std::function<void(Node*)> cb);

    static inline uint32_t total_uid = 0;
    uint32_t uid = 0;

    bool is_typed = false;

    void trig(Event event);

    Node* top();

    void up();

    void down();
    void run();

    std::string nameSTL();

    std::set<Node*> referings;

    auto begin() { return childrens.begin(); }

    auto end() { return childrens.end(); }

    template <typename T>
    void onadd(std::function<Node*(Node*,Node*)> cb) { onadd_cb[typeid(T)] = cb; }


    std::map<Event,std::function<void(Node*)>> on_cb;
    std::map<TypeIndex, std::function<Node*(Node*,Node*)>> onadd_cb;
    static inline std::map<Event, std::map<TypeIndex, void*>> ontyped;
    static inline std::map<TypeIndex, std::map<TypeIndex, std::function<Node*(Node*,Node*)>>> onaddtyped_cb;
    static inline std::map<TypeIndex, std::function<void*(void*)>> upcast_lists;
    static inline std::map<TypeIndex, std::function<void(void*)>> delete_lists;
    static inline std::map<TypeIndex, TypeIndex> is_lists;

    Node* operator[](int id);
    Node* operator[](std::string name);

    static inline Node* selected = nullptr;
    

    TypeIndex stored_type = typeid(Node);

    void* void_ptr = nullptr;

    bool owned;

    template <typename U>
    Node* addPtr(void* ptr) { 
        
        auto out = new Node(ptr,typeid(U), false);

        return add(out); 
        
    }

    template <typename U, typename... Args>
    Node* addOwnr(Args&&... args) {

        if (delete_lists.find(typeid(U)) == delete_lists.end()) delete_lists.emplace(typeid(U),[&](void* t){ delete (U*)t; });
        
        auto ptr = new U(std::forward<Args>(args)...);

        auto NEW_U = new Node(ptr, typeid(U), true);

        if (!add(NEW_U)) { delete ptr; delete NEW_U; return nullptr; } // et delete NEW_U non ?

        return NEW_U;

    }

    template <typename U>
    void each(std::function<void(Node*, U*)> cb) { 
        each_([&](Node* n) { 
            U* isa = n->is_a_nowarning<U>(); 
            if (isa) 
                cb(n,isa); 
        }); 
    }

    template <typename U>
    U* is_a() {

        auto x = is_a_nowarning<U>();

        if (!x) { PLOGW << "\"" << name() << "\" is " << type_name() << " not a " << boost::typeindex::type_id_with_cvr<U>().pretty_name() << " ! "; }

        return x;

    }

    template <typename U>
    U* is_a_nowarning() {

        TypeIndex t = stored_type;

        std::string t_ = stored_type.pretty_name();

        void* out = void_ptr ;

        while (true) {

            std::string t_ = stored_type.pretty_name();

            if (typeid(U) == t || is_lists.find(t) == is_lists.end()) break;

            out = upcast_lists[t](void_ptr);

            t = is_lists.at(t);
        
        }

        if (typeid(U) != t) return nullptr;
        
        return (U*)out; 
        
    }
    
private:

    Node* parent_node = nullptr;

    bool has_changed = false;

};


template <typename T>

struct NODE {
    
    static inline std::map<Node::Event, std::function<void(Node*,T*)>> on_cb;

    template <typename U>
    static inline void  is_a() { 
        
        Node::is_lists.emplace(typeid(T),typeid(U));

        Node::upcast_lists.emplace(typeid(T),[&](void* t){ return (U*)(T*)t; });
        
    }

    template <typename U>
    static void allow() { 

        NODE<T>::onadd<U>([](Node*_this,Node*node){ return node; }); 

    }

    template <typename U>
    static void onadd(std::function<Node*(Node*,Node*)> cb) { Node::onaddtyped_cb[typeid(T)][typeid(U)] = cb;  }

    static void on(Node::Event event, std::function<void(Node*,T*)> cb) { on_cb[event] = cb; Node::ontyped[event][typeid(T)] = &on_cb[event]; }

};
