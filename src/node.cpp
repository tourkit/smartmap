#include "node.hpp"
#include "file.hpp"
#include "log.hpp"


    Node::Node(std::string name, std::vector<uint32_t> color) : name(name), color(color) {

        PLOGV << name;
        
    }

    Node::~Node() {

        auto t_childrens = childrens;
        for (auto c : t_childrens) delete c;
        t_childrens.resize(0);

        if (parent_node) parent_node->remove(this);

        if (dtor) dtor(parent_node);

        PLOGV << "~" << name;

    }   

    Node* Node::add(Node* n)  { 

         n->parent(this);

        return n;

     }


    void Node::import(std::string path) {

        File file(path);

        import(&file);

    }

    Node *Node::parent() { return parent_node; }

    void Node::parent(Node* parent_node) {  
        
        if (this->parent_node == parent_node) return;

        if (this->parent_node) this->parent_node->remove(this);
        
        this->parent_node = parent_node;

        if (!parent_node) return;

        parent_node->childrens.push_back(this);
    
    }

    void Node::update() { if (parent_node) parent_node->update(); }

    void Node::remove(Node *child) {  

        auto it = std::find(childrens.begin(), childrens.end(), child);

        if ( std::distance(childrens.begin(), it) < 0 ) {
            
            PLOGD << "could not delete, didnt found";
            return; 
            
        }

        childrens.erase(it);

        update();

    }

    uint32_t Node::index() { 

        auto it = std::find(parent_node->childrens.begin(), parent_node->childrens.end(), this);

        return std::distance(parent_node->childrens.begin(), it);

    }

    void Node::runCB() { }

    void Node::run() { 

        for (auto c : childrens) c->run();

        runCB();
        
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



// struct GroupNode : Node {

//     GroupNode() : Node{"group"} {}

// };

