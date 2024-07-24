#include "node.hpp"
#include "log.hpp"
#include "buffer.hpp"
#include "ubo.hpp"
#include "utils.hpp"
#include <boost/type_index.hpp>


Node::Node(void* ptr, boost::typeindex::type_index type, bool owned) : void_ptr(ptr), stored_type(type), owned(owned) {

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
    EASY_TYPE_DEBUG = type().name();    
    EASY_TYPE_DEBUG += " " + type_name();
    #endif

    trigcreate();

}

const std::string& Node::name() { return name_v; }

Node* Node::name(std::string value) { name_v = value; update();  return this;}

Node::~Node() {

    auto t_childrens = childrens;
    for (auto c : childrens) delete c;

    t_childrens = hidden_childrens;
    for (auto c : t_childrens) delete c;

    for (auto x : pool) for (auto r : x->referings) if (r == this) { x->referings.erase(r); break; }


    if (ondelete_cb) ondelete_cb(this);

    if (parent_node) parent_node->remove(this);

    trigdelete();

    // if (owned) delete void_ptr; // how ?

    pool.erase(this);
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

void Node::
addList(NodeList *nodes) { for (auto n : *nodes) add(n); }

Node* Node::add(void* node_v)  {

        auto n = (Node*)node_v;

        if (n->parent() == this) return nullptr;

        bool found = false;

        boost::typeindex::type_index t = type();
        boost::typeindex::type_index u = n->type();
        
        while (true) { // find all derived onadds
            
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
        
        // if (!found) return nullptr;

        if (n == node_v) {

            PLOGV << type_name() << "::" << name() << " add " << n->type_name() << "::" << n->name();

            if (n->parent() == this) return nullptr;

            if (onadd_cb.find(n->type()) != onadd_cb.end()) {

                n = onadd_cb[n->type()](this,n);

                if (!n) return nullptr;

            }

            if (n == node_v) n->parent(this);

            else n->referings.insert(this); // that I think is an overzstatement, inst an alweeays fact. sdhoudl ne handled per callback // no !

            // update();
            n->trigchange();
            return n;

        }else {

            n->referings.insert(n); // that I think is an overzstatement, inst an alweeays fact. sdhoudl ne handled per callback
            return n;
        }


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

Node* Node::ondelete(std::function<void(Node*)> cb) { ondelete_cb = cb; return this; }
Node* Node::onchange(std::function<void(Node*)> cb) { onchange_cb = cb; return this; }
Node* Node::onrun(std::function<void(Node*)> cb) { onrun_cb = cb; return this; }

void Node::runCB(std::function<void(Node*)> cb) {

    for (auto c:childrens) c->runCB(cb);

    if(cb) cb(this);

}

Node* Node::close() {

    open = false;
    return this;
}

Node* Node::hide() {

    parent_node->remove(this);

    parent()->hidden_childrens.push_back(this);

    return this;

}

void Node::bkpupdate() {

    //TOFIX // hein ?
    // engine.static_ubo.bkp();
    // engine.dynamic_ubo.bkp();

    update();

}

Node* Node::operator[](std::string name) { for (auto c : childrens) if (c->name() == name) return c; return nullptr; }

Node* Node::operator[](int id) { return childrens[id]; }

void Node::update() {

    PLOGV << type_name() << "::" << name();

    trigchange();

    if (parent_node) parent_node->update();

    if (referings.size() && *referings.begin())
        for (auto x : referings)
            if (x)
                x->update();

}

bool Node::remove(Node *child) {

    PLOGV << type_name() << "::" << name() << " remove " << child->type_name() << "::" << child->name();

    auto it = std::find(childrens.begin(), childrens.end(), child);

    if ( std::distance(childrens.begin(), it) < 0 ) {

        PLOGI << "could not delete, didnt found";
        return false;

    }

    childrens.erase(it);

    update();

    return true;

}

uint32_t Node::index() {

    auto it = std::find(parent_node->childrens.begin(), parent_node->childrens.end(), this);

    return std::distance(parent_node->childrens.begin(), it);

}

void Node::run() {

    if (!is_active ) return; 

    boost::typeindex::type_index t = stored_type;

    void* out = void_ptr;

    if (out) while (true) {

        if (onruntyped.find(t) != onruntyped.end()) (*(std::function<void(Node*,void*)>*)
        
            onruntyped.at(t))(this,out);  

        if (Node::is_lists.find(t) == Node::is_lists.end()) break;

        out = upcast_lists[t](out);
        t = Node::is_lists.at(t);
    
    }

    for (auto c : childrens) c->run();

}

void Node::trigchange()  { 
        
        auto t = stored_type;

        void* out = void_ptr;

        while (true) {

            if (onchangetyped.find(t) != onchangetyped.end()) (*(std::function<void(Node*,void*)>*)
            
                onchangetyped.at(t))(this,out);  

            if (Node::is_lists.find(t) == Node::is_lists.end()) break;

            out = upcast_lists[t](out);

            t = Node::is_lists.at(t);
        
        }
        
    }
void Node::trigdelete()  { 
        
    auto t = stored_type;

    void* out = void_ptr;

    while (true) {

        if (ondeletetyped.find(t) != ondeletetyped.end()) (*(std::function<void(Node*,void*)>*)
        
            ondeletetyped.at(t))(this,out);  

        if (Node::is_lists.find(t) == Node::is_lists.end()) break;

        out = upcast_lists[t](out);

        t = Node::is_lists.at(t);
    
    }
    
}
void Node::trigcreate()  { 
        
    auto t = stored_type;

    void* out = void_ptr;

    while (true) {

        if (oncreatetyped.find(t) != oncreatetyped.end()) (*(std::function<void(Node*,void*)>*)
        
            oncreatetyped.at(t))(this,out);  

        if (Node::is_lists.find(t) == Node::is_lists.end()) break;

        out = upcast_lists[t](out);

        t = Node::is_lists.at(t);
    
    }
    
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
