#include "node.hpp"
#include "effector.hpp"
#include "log.hpp"
#include "buffer.hpp"
#include "ubo.hpp"
#include "utils.hpp"

#include "editor.hpp"

Node::Node(void* ptr, TypeIndex type, bool owned) : void_ptr(ptr), stored_type(type), owned(owned) {

    name_v = (type_name());

    init();

}

Node::Node(std::string name, ImVec4 color) : name_v(name), color(color) {

    init();

}

void Node::init() {

    uid = total_uid++;

    pool.insert(this);
    PLOGV << "#" << name();

    #ifdef ROCH
    TYPE = type_name();
    #endif

    trig(Node::CREATE);

}

const std::string& Node::name() { return name_v; }

Node* Node::name(std::string value) { 
    name_v = value; 
    update();  
    return this;
}

Node::~Node() {

    auto t_childrens = childrens;
    for (auto c : t_childrens) 
        delete c;

    t_childrens = hidden_childrens;
    for (auto c : t_childrens) delete c;

    for (auto x : pool) for (auto r : x->referings) if (r == this) { x->referings.erase(r); break; }

    if (parent_node) parent_node->remove(this);

    pool.erase(this);

    trig(Event::DESTROY);

    if (owned) 
        deletetyped_cb[stored_type](void_ptr);

    PLOGV << "~" << name();

}

Node* Node::find(std::vector<std::string> names) {

    auto traget = names.back();

    for (auto c : childrens) {

        if (!strcmp(names.back().c_str(), c->name().c_str())) {

            if (names.size()== 1) return c;

            auto parent = c;

            for (int i = names.size()-2; i >= 0; i--) if (!strcmp(parent->parent()->name().c_str(),names[i].c_str())) { parent = parent->parent(); }else{ c = nullptr; break; }

            if (c) return c;

        }

    }

    if (names.size()) for (auto c : childrens) {

        auto x = c->find(names);

        if (x) return x;

    }

    return nullptr;

}


Node* Node::find(std::string  name) {

    if (!name.length()) return nullptr;

    return find(split(name));

}

Node* Node::active(bool value) { is_active = value; return this; }


Node* Node::top() { auto top = this; while(top->parent()) { top = top->parent(); } return top; }

void Node::addList(std::vector<Node*> *nodes) { for (auto n : *nodes) add(n); }

std::vector<TypeIndex> Node::isList(TypeIndex t) {

    std::vector<TypeIndex> types = {t};

    auto &is_lists = Node::is_lists;

    if (is_lists.find(t) != is_lists.end()) 
        for (auto t : is_lists[t])  
            for (auto x : isList(t.first) )
                types.push_back(x);

    return types;

}

Node* Node::add_typed(TypeIndex t, TypeIndex u, Node* to_add, void* ptr) {



    return to_add;

}

Node* Node::add(void* node_v)  {

    auto og = (Node*)node_v;
    auto n = og;

    if (!n || n == this || n->parent() == this) return nullptr;

    PLOGV << type_name() << "::" << name() << " add " << n->type_name() << "::" << n->name();

    bool callback_ = false;

    auto is_u = isList(n->type());
    is_u.push_back(typeid(AnyNode));

    for (auto u_ : is_u) {
        std::string u_NAME = u_.pretty_name();
        if (onadd_cb.find(u_) != onadd_cb.end()) {
            n = onadd_cb[u_](this,n);
            callback_ = true;
        }
    }

    if (!n) { 
        return nullptr;
    }

    auto is_t = isList(type());

    for (auto t_ : is_t){

        bool break_ = false;
        for (auto u_ : is_u){

            std::string t_NAME = t_.pretty_name();
            std::string u_NAME = u_.pretty_name();

            if (onaddtyped_cb.find(t_) != onaddtyped_cb.end() && onaddtyped_cb.at(t_).find(u_) != onaddtyped_cb.at(t_).end()) {
                n = onaddtyped_cb[t_][u_](this,n);    
                break_ = true;
                callback_ = true;
                break;
            }
        }

        if (break_)
            break;
    
    }

    if (!callback_){
        
        PLOGW << "no cb for " << og->name() << " ("<<  og->type_name() << ") in " << name() << " (" << type_name() << ")"; 
        return nullptr;
    
    }

    if (n != node_v) {
        if (n) 
            og->referings.insert(n);
        else
            { PLOGW << name() << " can't add " << og->name(); }
        return n;
    }
    
    n->parent(this);

    update();

    return n;

}

std::string Node::nameSTL(){ if (parent()) { return parent()->name() + "::" + name(); } return name(); }

Node *Node::parent() { return parent_node; }

Node* Node::select(){ selected = this; return this;  }

void Node::parent(Node* parent_node) {

    if (this->parent_node == parent_node) return;

    if (this->parent_node) this->parent_node->remove(this);

    this->parent_node = parent_node;

    if (!parent_node) return;

    is_active = parent_node->is_active;

    parent_node->childrens.push_back(this);

}

Node* Node::on(Event event, std::function<void(Node*)> cb) { on_cb[event] = cb; return this; }


Node* Node::close() {

    open = false;
    return this;
}

Node* Node::hide() {

    parent_node->remove(this);

    parent()->hidden_childrens.push_back(this);

    return this;

}

Node* Node::operator[](std::string name) { return find(name); }

Node* Node::operator[](int id) { return childrens[id]; }

void Node::update() {

    PLOGV << type_name() << "::" << name();

    trig(Event::CHANGE);

    if (parent_node) parent_node->update();

    if (referings.size() && *referings.begin())
        for (auto x : referings)
            if (x)
                x->update();

}


void Node::each_untyped(std::function<void(Node*)> cb) { 
    
    for (auto c : childrens) 
        c->each_untyped(cb); 
    
    cb(this);  

}

bool Node::remove(Node *child) {

    PLOGV << type_name() << "::" << name() << " remove " << child->type_name() << "::" << child->name();

    auto it = std::find(childrens.begin(), childrens.end(), child);

    if ( std::distance(childrens.begin(), it) < 0 ) {

        PLOGI << "could not delete, didnt found";
        return false;

    }

    childrens.erase(it);

    if (parent()) parent()->update();

    return true;

}

void Node::run() { 
    
    if (!is_active) 
        return; 

    trig(Node::RUN); 

    for (auto x : childrens) 
        x->run(); 

}

uint32_t Node::index() {

    auto it = std::find(parent_node->childrens.begin(), parent_node->childrens.end(), this);

    return std::distance(parent_node->childrens.begin(), it);

}


static std::string event_name(Node::Event event){
    switch (event) {
        case Node::CREATE: return "CREATE";
        case Node::CHANGE: return "CHANGE";
        case Node::RUN: return "RUN";
        case Node::DESTROY: return "DELETE";
    }

    return "UNKNOWN";
}

void Node::trig_typed(Node::Event e, TypeIndex t, void* out) {

#ifdef ROCH
    std::string t_NAME = t.pretty_name();
    Wrappy* ww = (Wrappy*)out;
#endif

    if (is_lists.find(t) != is_lists.end()) 
        for (auto is : is_lists[t]) {

            std::string is_NAME = is.first.pretty_name();
            trig_typed(e, is.first, is.second(out));
        }

    if (ontyped_cb[e].find(t) != ontyped_cb[e].end()) 
        (*(std::function<void(Node*,void*)>*) ontyped_cb[e].at(t))(this,out);  

}

void Node::trig(Event e)  { 

    trig_typed(e, type(), void_ptr);

    if (on_cb.find(e) != on_cb.end()) 
        on_cb[e](this);
    
}


void Node::editor() { 

    if(EDITOR::ptr.find(stored_type) != EDITOR::ptr.end()) 

        (*(std::function<void(Node*,void*)>*)EDITOR::ptr[stored_type])(this,this->void_ptr);

}

void* Node::is_a_untyped(TypeIndex t, TypeIndex u, void* out) {

#ifdef ROCH
    std::string t_NAME = t.pretty_name();
    std::string u_NAME = u.pretty_name();
#endif

    if (t == u) 
        return out;

    if (is_lists.find(t) != is_lists.end()) 

        for (auto t_ : is_lists[t]) {

            if (t_.first == u) 
                return t_.second(out);

            else
                if (auto result = is_a_untyped(t_.first,u, t_.second(out))) 
                    return result;

        }

    return nullptr;

}