#include "node.hpp"
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

Node* Node::name(std::string value) { name_v = value; update();  return this;}

Node::~Node() {

    auto t_childrens = childrens;
    for (auto c : childrens) delete c;

    t_childrens = hidden_childrens;
    for (auto c : t_childrens) delete c;

    for (auto x : pool) for (auto r : x->referings) if (r == this) { x->referings.erase(r); break; }

    if (parent_node) parent_node->remove(this);

    pool.erase(this);

    trig(Event::DESTROY);

    if (owned) delete_lists[stored_type](void_ptr);

    PLOGV << "~" << name();

}

Node* Node::child(std::vector<std::string> names) {

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

        auto x = c->child(names);

        if (x) return x;

    }

    return nullptr;

}


Node* Node::child(std::string  name) {

    return child(split(name));

}

Node* Node::active(bool value) { is_active = value; return this; }


Node* Node::top() { auto top = this; while(top->parent()) { top = top->parent(); } return top; }

void Node::addList(std::vector<Node*> *nodes) { for (auto n : *nodes) add(n); }

Node* Node::add(void* node_v)  {

    auto n = (Node*)node_v;

    if (!n || n == this || n->parent() == this) return nullptr;

    PLOGV << type_name() << "::" << name() << " add " << n->type_name() << "::" << n->name();

    bool found = false;

    TypeIndex t = type();
    
    while (true) { // find all derived onadds
    
        TypeIndex u = n->type();
        
        while (true) { // find all derived onadds
    
            std::string _t = t.name();
            std::string _u = u.name();

            if (onaddtyped_cb.find(t) != onaddtyped_cb.end() && onaddtyped_cb.at(t).find(u) != onaddtyped_cb.at(t).end()) { found = true;
                
                n = onaddtyped_cb[t][u](this,n);

                if (n == this) return nullptr;

            }
            
            if (Node::is_lists.find(u) == Node::is_lists.end()) break;

            u = Node::is_lists.at(u);

        }
        
        if (Node::is_lists.find(t) == Node::is_lists.end()) break;

        t = Node::is_lists.at(t);

    }
    
    // if (!found) return nullptr; // test for no auto add

    if (onadd_cb.find(n->type()) != onadd_cb.end()) {
        n = onadd_cb[n->type()](this,n);

        if (n == this) return nullptr;

    } 

    n->parent(this);

    update();
    
    if (n != node_v) ((Node*)node_v)->referings.insert(n);

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

Node* Node::operator[](std::string name) { return child(name); }

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


void Node::each_(std::function<void(Node*)> cb) { 
    
    for (auto c : childrens) 
        c->each_(cb); 
    
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

void Node::trig(Event e)  { 
        
    auto t = stored_type;

    void* out = void_ptr;

    while (true) {

        auto t_ = t.name();

        if (ontyped[e].find(t) != ontyped[e].end()) {
            
            // PLOGV << event_name(e) << " : " << name() << " is " << type_name() << " as " << t_ ;
        
            (*(std::function<void(Node*,void*)>*) ontyped[e].at(t))(this,out);  

        }
        
        if (on_cb.find(e) != on_cb.end()) 
            on_cb[e](this);
        
        if (Node::is_lists.find(t) == Node::is_lists.end()) break;

        out = upcast_lists[t](out);

        t = Node::is_lists.at(t);
    
    }
    
}

void Node::editor() { 

    if(EDITOR::ptr.find(stored_type) != EDITOR::ptr.end()) 

        (*(std::function<void(Node*,void*)>*)EDITOR::ptr[stored_type])(this,this->void_ptr);

}

void Node::up() {

    if (!parent_node) return;

    auto it = std::find(parent_node->childrens.begin(), parent_node->childrens.end(), this);
    int index = std::distance(parent_node->childrens.begin(), it);

    if(index<1) return;

    parent_node->childrens.erase(it);
    parent_node->childrens.insert(parent_node->childrens.begin() + index - 1, this);

}

void Node::down() {

    if (!parent_node) return;

    auto it = std::find(parent_node->childrens.begin(), parent_node->childrens.end(), this);
    int index = std::distance(parent_node->childrens.begin(), it);

    if(index > parent_node->childrens.size()-2) return;

    parent_node->childrens.erase(it);
    parent_node->childrens.insert(parent_node->childrens.begin() + index + 1, this);

}
