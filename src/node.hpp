#pragma once

#include <cstring>
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <type_traits>
#include <typeinfo>
#include <functional>

#include "file.hpp"
#include "editor.hpp"
#include "folder.hpp"

#include <boost/type_index.hpp>

#include "imgui/imgui.h"

struct Node;

using NodeList = std::vector<Node*>;

struct Node {

    std::string name_v;
#ifdef ROCH
public:
    std::string EASY_TYPE_DEBUG = "NODE";
#endif
    const std::string& name();
    Node* name(std::string value);

    ImVec4 color;

    std::vector<Node*> childrens;

    std::vector<Node*> hidden_childrens;

    bool locked = false, loaded = false, hidden = false, open = true, is_active = false;

    static inline std::set<Node*> pool;

    Node(std::string name = "node", ImVec4 color = {1,1,1,1});
    Node(void* ptr, boost::typeindex::type_index type, bool owned);


    Node(Node* other) : Node(other->void_ptr, other->stored_type, other->owned) {  }

    virtual ~Node();

    void init();

    // void editor() { if(Editor<T>::cb)
    // Editor<T>::cb(this,this->void_ptr);
    // }

    virtual void run();

    virtual void update();

    void bkpupdate();

    Node* add(void *node);

    void parent(Node* parent_node);

    Node* parent();

    Node* child(std::string  name);
    Node* child(std::vector<std::string> names);

    uint32_t index();

    void addList(NodeList *nodes);

    Node* hide();

    Node* close();

    Node* active(bool value);

    Node* select();

    bool remove(Node *child);

    boost::typeindex::type_index type() { return stored_type; }

    virtual std::string type_name() { return type().pretty_name();  }

    Node* onchange(std::function<void(Node*)> cb = nullptr);

    Node* ondelete(std::function<void(Node*)> cb = nullptr);

    Node* onrun(std::function<void(Node*)> cb = nullptr);


    // template <typename V>
    // void each(std::function<void(Node*, V*)> cb) { for (auto c : childrens) { Node* isa = c->is_a<V>(); if (isa) cb(c,isa); } }


    void each_(std::function<void(Node*)> cb) { for (auto c : childrens) { ((Node*)c)->each_(cb); } cb(this);  }

    static inline uint32_t total_uid = 0;
    uint32_t uid = 0;

    bool is_typed = false;

    void trigchange();

    void trigcreate();
    
    void trigdelete();

    Node* top();

    void up();

    void down();

    std::string nameSTL();

    std::set<Node*> referings;


    auto begin() { return childrens.begin(); }

    auto end() { return childrens.end(); }

    std::function<void(Node*)> onchange_cb = nullptr;
    std::function<void(Node*)> onrun_cb = nullptr;
    std::function<void(Node*)> ondelete_cb = nullptr;

    template <typename T>
    void onadd(std::function<Node*(Node*,Node*)> cb) { onadd_cb[typeid(T)] = cb; }

    template <typename U>
    static void editor(std::function<void(Node*,U*)> cb) { Editor<U>::cb = cb; }

    std::map<boost::typeindex::type_index, std::function<Node*(Node*,Node*)>> onadd_cb;

    static inline std::map<boost::typeindex::type_index,std::map<boost::typeindex::type_index, std::function<Node*(Node*,Node*)>>> onaddtyped_cb;

    Node* operator[](int id);
    Node* operator[](std::string name);

    static inline Node* selected = nullptr;
    
    static inline std::map< boost::typeindex::type_index, boost::typeindex::type_index> is_lists;
    static inline std::map< boost::typeindex::type_index, std::function<void*(void*)> > upcast_lists;
    
    static inline std::map<boost::typeindex::type_index, void*> onchangetyped;
    static inline std::map<boost::typeindex::type_index, void*> oncreatetyped;
    static inline std::map<boost::typeindex::type_index, void*> ondeletetyped;
    static inline std::map<boost::typeindex::type_index, void*> onruntyped;

    boost::typeindex::type_index stored_type = typeid(Node);

    void* void_ptr = nullptr;

    bool owned;

    template <typename U>
    Node* addPtr(void* ptr) { 
        
        auto out = new Node(ptr,typeid(U), false);

        return add(out); 
        
    }

    template <typename U, typename... Args>
    Node* addOwnr(Args&&... args) {

        auto ptr = new U(std::forward<Args>(args)...);

        auto NEW_U = new Node(ptr, typeid(U), true);

        if (!add(NEW_U)) { delete ptr; return nullptr; } // et delete NEW_U non ?

        return NEW_U;

    }

    template <typename U>
    void each(std::function<void(Node*, U*)> cb) { each_([&](Node* n) { auto isa = n->is_a_nowarning<U>(); if (isa) cb(n,isa); }); }

    template <typename U>
    U* is_a() {

        auto x = is_a_nowarning<U>();

        if (!x) { PLOGW << "\"" << name() << "\" is " << type_name() << " not a " << boost::typeindex::type_id_with_cvr<U>().pretty_name() << " ! "; }

        return x;

    }

    template <typename U>
    U* is_a_nowarning() {

        boost::typeindex::type_index t = stored_type;

        void* out = typeid(U) == t ? void_ptr : nullptr;


        while (true) {

            if (typeid(U) == t || is_lists.find(t) == is_lists.end()) break;

            out = upcast_lists[t](void_ptr);

            t = is_lists.at(t);
        
        }

        return (U*)out; 
        
    }

private:

    Node* parent_node = nullptr;

    bool has_changed = false;

    void runCB(std::function<void(Node*)> cb = nullptr);

    bool isNode() { return stored_type == boost::typeindex::type_index(typeid(Node)); }

};


template <typename T>

struct NODE {

    static inline std::function<void(Node*,T*)> ondelete_cb = nullptr;
    static inline std::function<void(Node*,T*)> oncreate_cb = nullptr;
    static inline std::function<void(Node*,T*)> onrun_cb = nullptr;
    static inline std::function<void(Node*,T*)> onchange_cb = nullptr;

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
    static void ondelete(std::function<void(Node*,T*)> cb) { ondelete_cb = cb;  Node::ondeletetyped[typeid(T)] = &ondelete_cb; }
    static void oncreate(std::function<void(Node*,T*)> cb) { oncreate_cb = cb;  Node::oncreatetyped[typeid(T)] = &oncreate_cb; }
    static void onchange(std::function<void(Node*,T*)> cb) { onchange_cb = cb; Node::onchangetyped[typeid(T)] = &onchange_cb; }
    static void onrun(std::function<void(Node*,T*)> cb) { onrun_cb = cb; Node::onruntyped[typeid(T)] = &onrun_cb; }

};
