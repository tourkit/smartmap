#include "node.hpp"
#include "log.hpp"
#include "engine.hpp"

    UntypedNode::UntypedNode(std::string name, glm::vec4 color) : name(name), color(color) {

        uid = uid++;
        
    }


    UntypedNode::~UntypedNode() {

        top()->runCB([this](Node* node){ for (auto r : node->referings) if (r == this) r = nullptr; });

        auto t_childrens = childrens;
        for (auto c : t_childrens) delete c;

        t_childrens = hidden_childrens;
        for (auto c : t_childrens) delete c;

        if (parent_node) parent_node->remove(node());

        if (dtor) dtor(parent_node);

        PLOGD << "~" << name;

    }   

    Node* UntypedNode::child(const char* name) {

        for (auto c : childrens) if (!strcmp(name,c->name.c_str())) return c;
        
        return nullptr; 
    }

    Node* UntypedNode::node() { return (Node*)this; }

    Node* UntypedNode::top() { auto top = node(); while(top->parent()) { top = top->parent(); } return top; }

    void UntypedNode::addList(NodeList *nodes) { for (auto n : *nodes) add(n); }

    Node* UntypedNode::add(void* node_v)  {
        
        auto n = (Node*)node_v; 
        
        if (n->parent() == node()) return nullptr;

        if (onadd_cb.find(n->type()) != onadd_cb.end()) {

            n = onadd_cb[n->type()](this->node(),n->node());

            if (n->node() == this->node()) return nullptr;

        }

        n->parent(node());

        return n;

     }


    Node *UntypedNode::parent() { return parent_node; }

    Node* UntypedNode::select(){ engine.selected = node(); return node();  }
    
    void UntypedNode::parent(Node* parent_node) {  
        
        if (this->parent_node == parent_node) return;

        if (this->parent_node) this->parent_node->remove(node());
        
        this->parent_node = parent_node;

        if (!parent_node) return;

        active = parent_node->active;

        parent_node->childrens.push_back(node());
    
    }

    void UntypedNode::onchange(std::function<void(Node*)> cb) { onchange_cb = cb; }
    void UntypedNode::onrun(std::function<void(Node*)> cb) { onrun_cb = cb; }
    
    void UntypedNode::runCB(std::function<void(Node*)> cb) {

        for (auto c:childrens) c->runCB(cb);

        if(cb) cb(node());

    }

    Node* UntypedNode::close() { 

        open = false;
        return node();
    }

    Node* UntypedNode::hide() { 

        parent_node->remove(node());

        parent()->hidden_childrens.push_back(node()); 

        return node();

    }

    void UntypedNode::update() { 
                
        if (parent_node) parent_node->update(); 
        
        top()->runCB([this](Node* curr){ for (auto r : curr->referings)if (r == this) curr->update(); });

        if (onchange_cb) onchange_cb(node());
        
    }

    void UntypedNode::remove(Node *child) {  

        auto it = std::find(childrens.begin(), childrens.end(), child);

        if ( std::distance(childrens.begin(), it) < 0 ) {
            
            PLOGI << "could not delete, didnt found";
            return; 
            
        }

        childrens.erase(it);

        update();

    }

    NodeList* UntypedNode::updateRefs(Node* of) { 

        return nullptr;

    }

    uint32_t UntypedNode::index() { 

        auto it = std::find(parent_node->childrens.begin(), parent_node->childrens.end(), this);

        return std::distance(parent_node->childrens.begin(), it);

    }

    void UntypedNode::run() { 

        if (!active && this != engine.tree) return;

        if (onrun_cb) onrun_cb(node());

        for (auto c : childrens) c->run();
        
    }

    void UntypedNode::trigchange() { if (onchange_cb) onchange_cb(node()); }

    void UntypedNode::up() { 
        
        if (!parent_node) return;

        auto it = std::find(parent_node->childrens.begin(), parent_node->childrens.end(), node());
        int index = std::distance(parent_node->childrens.begin(), it);

        if(index<1) return; 

        parent_node->childrens.erase(it);
        parent_node->childrens.insert(parent_node->childrens.begin() + index - 1, node());
    
    }

    void UntypedNode::down() {

        if (!parent_node) return;
        
        auto it = std::find(parent_node->childrens.begin(), parent_node->childrens.end(), node());
        int index = std::distance(parent_node->childrens.begin(), it);  

        if(index > parent_node->childrens.size()-2) return;

        parent_node->childrens.erase(it);
        parent_node->childrens.insert(parent_node->childrens.begin() + index + 1, node()); 

    }
