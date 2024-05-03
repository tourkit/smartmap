#pragma once

#include <cstring>
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <functional>

#include "log.hpp"
#include "file.hpp"
#include "editor.hpp"
#include "directory.hpp"

#include <boost/type_index.hpp>

#include "imgui/imgui.h"

struct Node;

using NodeList = std::vector<Node*>;

struct UntypedNode {

    std::string name_v;
#ifdef ROCH
public:
    std::string EASY_TYPE_DEBUG = "NODE";
#endif
    std::string name();
    void name(std::string value);

    ImVec4 color;

    std::vector<Node*> childrens;

    std::vector<Node*> hidden_childrens;

    bool locked = false, loaded = false, hidden = false, open = true, is_active = false;

    static inline std::set<UntypedNode*> pool;

    UntypedNode(std::string name = "node", ImVec4 color = {1,1,1,1});

    virtual ~UntypedNode();

    virtual void editor() {}

    virtual void run();

    virtual void update();

    void bkpupdate();

    Node* node();

    virtual Node* add(void *node);

    void parent(Node* parent_node);

    Node* parent();

    Node* child(const char* name);
    Node* child(std::vector<std::string> names);

    uint32_t index();

    void addList(NodeList *nodes);

    Node* hide();

    Node* close();

    Node* active(bool value);

    Node* select();

    bool remove(Node *child);

    virtual std::type_index type() { return typeid(*this); }

    virtual std::string type_name() { return "Node"; }

    virtual void* ptr_untyped() { return this; }

    Node* onchange(std::function<void(Node*)> cb = nullptr);

    Node* ondelete(std::function<void(Node*)> cb = nullptr);

    Node* onrun(std::function<void(Node*)> cb = nullptr);

    template <typename U>
    U* is_a() { if (type() == typeid(U)) { return (U*)ptr_untyped(); }else { PLOGE << "NOT A << " << boost::typeindex::type_id_with_cvr<U>().pretty_name();return nullptr; } }

    template <typename V>
    void each(std::function<void(Node*, V*)> cb) { for (auto c : childrens) { auto isa = ((UntypedNode*)c)->is_a<V>(); if (isa) cb(c,isa); } }

    uint32_t uid = 0;

    bool is_typed = false;

    virtual void trigchange();

    Node* top();

    void up();

    void down();

    std::string namesdf();

    std::set<Node*> referings;


    auto begin() { return childrens.begin(); }

    auto end() { return childrens.end(); }

    std::function<void(Node*)> onchange_cb = nullptr;
    std::function<void(Node*)> onrun_cb = nullptr;
    std::function<void(Node*)> ondelete_cb = nullptr;

    template <typename T>
    void onadd(std::function<Node*(Node*,Node*)> cb) { onadd_cb[typeid(T)] = cb; }

    std::unordered_map<std::type_index, std::function<Node*(Node*,Node*)>> onadd_cb;

    static inline std::unordered_map<std::type_index,std::unordered_map<std::type_index, std::function<Node*(Node*,Node*)>>> onaddtyped_cb;

    Node* operator[](int id);

private:

    Node* parent_node = nullptr;

    bool has_changed = false;

    void runCB(std::function<void(Node*)> cb = nullptr);



};



struct Any {};
struct xxx { xxx(int x) {} };

template <typename T>
struct Ownr;

template <typename T>
struct TypedNode : UntypedNode {

    static inline std::function<void(Node*,T*)> ondelete_cb = nullptr;
    static inline std::function<void(Node*,T*)> oncreate_cb = nullptr;
    static inline std::function<void(Node*,T*)> onrun_cb = nullptr;
    static inline std::function<void(Node*,T*)> onchange_cb = nullptr;

    T* ptr;

    bool owned;

    std::type_index stored_type;


    TypedNode(void* ptr, bool owned = false) :

        UntypedNode((isNode()? ((UntypedNode*)ptr)->name_v : type_name())), ptr((T*)ptr), owned(owned), stored_type(typeid(T)) {

            if(oncreate_cb) { oncreate_cb(node(),this->ptr); }

            #ifdef ROCH
            EASY_TYPE_DEBUG = type().name();
            EASY_TYPE_DEBUG += " " + type_name();
            #endif

    }


    T* get() { return ptr; }

    template <typename U>
    TypedNode<U>* get() { return (TypedNode<U>*)this; }

    std::type_index type() override { return stored_type; }

    void* ptr_untyped() override { return ptr; }

    operator T*() { return ptr; }

    void update() override {

        if(onchange_cb) { onchange_cb(node(),this->ptr); }

        UntypedNode::update();

    }

    TypedNode<T>* select() { UntypedNode::select(); return this; }



    void trigchange() override {
        UntypedNode::trigchange();
        if(onchange_cb) onchange_cb(node(),this->ptr);
    }
    void run() override {

        UntypedNode::run();

        if(is_active && onrun_cb) { onrun_cb(node(),this->ptr); }

    }

    std::string type_name() override { return boost::typeindex::type_id<T>().pretty_name(); }

    Node* add(void *node_v) override {

        auto n = (TypedNode<Any>*)node_v;

        if (n->parent() == node()) return nullptr;

        if (onaddtyped_cb[type()].size()) {

            if (onaddtyped_cb[type()].find(n->type()) != onaddtyped_cb[type()].end()) {

                n = onaddtyped_cb[type()][n->type()](node(),n->node());

                if (!n) return nullptr;


            }

        }

        if (n == node_v) {return UntypedNode::add(n);}
        else {
            ((TypedNode<Any>*)node_v)->referings.insert(n->node());
            return n->node();
        }

    }


    TypedNode(TypedNode<Any>* other) : TypedNode<Any>(other->ptr) { stored_type = other->type(); }

    Node* addPtr(void* node_v) {

        auto node = (TypedNode<Any>*) node_v;

        return add(node);

    }

    template <typename U>
    TypedNode<U>* addPtr(void* ptr) { return (TypedNode<U>*)TypedNode<T>::add(new TypedNode<U>((U*)ptr)); }

    template <typename U, typename... Args>
    TypedNode<U>* addOwnr(Args&&... args) {

        auto ptr = new U(std::forward<Args>(args)...);

        auto NEW_U = new TypedNode<U>(ptr, true);

        if (!TypedNode<T>::add(NEW_U)) { delete ptr; return nullptr; } // et delete NEW_U non ?

        return NEW_U;

    }

    ~TypedNode() override {

        auto t_childrens = childrens;
        for (auto c : t_childrens) delete c;

        t_childrens = hidden_childrens;
        for (auto c : t_childrens) delete c;

        if (ondelete_cb) ondelete_cb(node(), ptr);

        if (owned) delete ptr;

    }

    void editor() override { if(Editor<T>::cb) Editor<T>::cb(node(),this->ptr); }

    template <typename U>
    TypedNode<T>* addFolder(std::string name, std::string path) {

        // auto folder = addOwnr<Any>();
        // folder->name = name;

        // auto dir = folder->TypedNode::addOwnr<Directory>(path);
        auto dir = TypedNode::addOwnr<Directory>(path);

        // dir->hide();

        for (auto f : dir->get()->list)  dir->node()->TypedNode::addOwnr<File>(f);

        // for (auto f : dir->childrens) folder->TypedNode::addOwnr<U>(f->TypedNode::is_a<File>());//->referings.insert(f); this shjouldnt hpn here

        return dir->node();
        // return folder;

    }

private:

    bool isNode() { return std::is_base_of<UntypedNode, T>::value; }

};



template <typename T>
struct Ownr : TypedNode<T> {

    template <typename... Args>
    Ownr(Args&&... args) : TypedNode<T>(new T(std::forward<Args>(args)...), true) { }

};

struct Node : TypedNode<Any> {

    static inline Node* onchange_payload = nullptr;

    Node(std::string name = "any") : TypedNode<Any>(this) { this->name(name); }

    template <typename U>
    static void editor(std::function<void(Node*,U*)> cb) { Editor<U>::cb = cb; }

    void editor() override {  }

};


template <typename T>
struct Ptr : TypedNode<T> { Ptr(T *ptr)  : TypedNode<T>(ptr)  { } };

template <typename T>
struct NODE : TypedNode<T> {

    NODE(T *ptr)  : TypedNode<T>(ptr)  { }

    template <typename U>
    static void onadd(std::function<Node*(Node*,Node*)> cb) { UntypedNode::onaddtyped_cb[typeid(T)][typeid(U)] = cb; }

    static void ondelete(std::function<void(Node*,T*)> cb) { TypedNode<T>::ondelete_cb = cb;  }
    static void oncreate(std::function<void(Node*,T*)> cb) { TypedNode<T>::oncreate_cb = cb;  }
    static void onchange(std::function<void(Node*,T*)> cb) { TypedNode<T>::onchange_cb = cb;  }
    static void onrun(std::function<void(Node*,T*)> cb) { TypedNode<T>::onrun_cb = cb;  }

};
