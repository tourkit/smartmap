#include "node.hpp"
#include "utils.hpp"

#include "editor.hpp"

Node::Node(void* ptr, TypeIndex type, bool owned) : void_ptr(ptr), stored_type(type), owned(owned) {

    name_v = (type_name());

    init();

}

Node::Node(std::string name, std::array<float,4> color) : color(color) {

    this->name(name);
    
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

    trig(Event::DESTROY);
      
    for (auto x : pool) for (auto r : x->referings) if (r == this) { x->referings.erase(r); break; }

    if (parent_node) parent_node->remove(this);

    pool.erase(this);

    if (owned) 
        dtortyped_cb[stored_type](void_ptr);


    PLOGV << "~" << name();

}

static bool find_obscure(std::vector<std::string>& names, Node* n,int max, int depth, std::vector<Node*>& find_list) {

    bool out = false;
        
    if (max == depth) {

        out = true;

        auto curr = n;

        for (int i = 0; i < names.size(); i++) {

            Node* found = nullptr;
            for (auto x : curr->childrens)
                if (x->name() == names[i]) {
                    found = x;
                    break;
                }

            if (!found) {

                curr = nullptr;

                break;

            }

            curr = found;
            
        }

        if (curr)
            ADD_UNIQUE<Node*>(find_list, curr);

    }

    for (auto x : n->childrens)
      if (find_obscure(names, x, max, depth+1, find_list))
        out = true;
    
    return out;
  
} 

Node* Node::find(std::vector<std::string> names) {

    find_list.clear();
    find_pos = 0;

    int i = 0;
    while(true) 
        if (!find_obscure(names, this, i++, 0,find_list)) 
            break;
    return find_next();

}


Node* Node::find_next() {

    if (!find_list.size() || find_pos > find_list.size()-1)
        return nullptr;

    return find_list[find_pos++];

}

Node* Node::find(std::string  name) {

    if (!name.length()) 
        return nullptr;

    return find(split(name));

}

Node* Node::active(bool value) { 

    is_active = value; 
    
    if (is_active)
        trig(Node::ACT);
    else
        trig(Node::DEACT);
    
    return this; 
}


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
    Node* n = og;

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

    if (!callback_ && n && n != Node::no_worry) { 
        
        auto is_t = isList(type());

        for (auto t_ : is_t){

            bool break_ = false;
            for (auto u_ : is_u){

                std::string t_NAME = t_.pretty_name();
                std::string u_NAME = u_.pretty_name();

                if (onaddtyped_cb.find(t_) != onaddtyped_cb.end() && onaddtyped_cb.at(t_).find(u_) != onaddtyped_cb.at(t_).end()) {

                    PLOGV << t_.pretty_name() << "::" << name() << " ONADD " << u_.pretty_name() << "::" << n->name();

                    n = onaddtyped_cb[t_][u_](this,n);    
                    break_ = true;
                    callback_ = true;
                    break;
                }
            }

            if (break_)
                break;
        
        }

    }
    if (!callback_){
        
        PLOGW << "no cb for " << og->name() << " ("<<  og->type_name() << ") in " << name() << " (" << type_name() << ")"; 
        return nullptr;
    
    }

    if (n != node_v) {

        if (n != no_worry) {

            if (n ) 
                og->referings.insert(n);
            
            else 
                { PLOGW << type_name() << "::" << name() << " couldn't add " << og->type_name() << "::" << og->name(); }
        }

        return n;
    }
    
    n->parent(this);

    return n;

}

std::string Node::nameSTL(int depth){ 
    if ((depth < 0 || depth ) && parent()) { 
        std::string out = parent()->nameSTL(depth-1);
        return out + (out.length()?"::":"") + name(); 
    } 
    return ""; 
    }

Node *Node::top_parent() { 
    
    Node* top_parent = this;
    
    while (top_parent->parent()) 
        top_parent = top_parent->parent(); 

    return top_parent; 

}

Node *Node::parent() { return parent_node; }

void Node::parent(Node* parent_node) {

    if (this->parent_node == parent_node) return;

    if (this->parent_node) this->parent_node->remove(this);

    this->parent_node = parent_node;

    if (!parent_node) return;

    is_active = parent_node->is_active;

    parent_node->childrens.push_back(this);

    update();

}

Node* Node::on(Event event, std::function<void(Node*)> cb) { on_cb[event] = cb; return this; }


Node* Node::close() {

    open = false;
    return this;
}

Node* Node::hide() {

    hidden = true;

    return this;

}

Node* Node::operator[](std::string name) { return find(name); }

Node* Node::operator[](int id) { return childrens[id]; }

void Node::update() {

    PLOGV << type_name() << "::" << name();

    error = false;

    trig(Event::CHANGE);

    if (parent_node && parent_node->referings.find(this) == parent_node->referings.end()) 
        parent_node->update();

    if (referings.size() && *referings.begin())
        for (auto x : referings)
            if (x)
                x->update();

}


Node* Node::each_untyped(std::function<Node*(Node*)> cb, int depth) { 
    
    Node* out = nullptr;

    for (auto c : childrens) {

        if (depth != 0){
        
            out = c->each_untyped(cb, depth-1);

            if (out == Node::Break) 
                return Node::Break;
        
        }
        
        out = cb(this);

        if (out == Node::Break) 
            return Node::Break;
        
    }
    
    return out;  

}

bool Node::remove(Node *child) {


    PLOGV << type_name() << "::" << name() << " remove " << child->type_name() << "::" << child->name();

    auto it = std::find(childrens.begin(), childrens.end(), child);

    if ( std::distance(childrens.begin(), it) < 0 ) {

        PLOGI << "could not delete, didnt found";
        return false;

    }

    childrens.erase(it);

    if (child->type() != typeid(None) && !child->hidden) 
        update();

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
        case Node::DESTROY: return "DESTROY";
        case Node::ACT: return "ACTIVATE";
        case Node::DEACT: return "DEACTIVATE";
    }

    return "UNKNOWN";
}

void Node::trig_typed(Node::Event e, TypeIndex t, void* out) {

    std::string t_NAME = t.pretty_name();

    if (is_lists.find(t) != is_lists.end()) 
        for (auto is : is_lists[t]) {

            std::string is_NAME = is.first.pretty_name();
            trig_typed(e, is.first, is.second(out));
        }

    if (ontyped_cb[e].find(t) != ontyped_cb[e].end()) {
        
        if (e != RUN)
            { PLOGV << event_name(e)  << " " << t.pretty_name() << "::" << name(); }

        (*(std::function<void(Node*,void*)>*) ontyped_cb[e].at(t))(this,out);  
        
    }
}

void Node::trig(Event e)  { 

    trig_typed(e, type(), void_ptr);
    
    trig_typed(e, typeid(AnyNode), void_ptr);

    if (on_cb.find(e) != on_cb.end()) 
        on_cb[e](this);
    
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