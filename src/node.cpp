#include "node.hpp"


    Node::Node(std::string name, std::vector<uint32_t> color) { 

        this->name = name;
        
        this->color = color;

    }

    Node::~Node() {

        for (auto c : childrens) c->parent(nullptr);

        if (parent_node) parent_node->remove(this);

        PLOGD << "node " << name << " deleted";

    }

    void Node::parent(Node* parent_node) {  
        
        if (this->parent_node == parent_node) return;

        if (this->parent_node) this->parent_node->remove(this);
        
        this->parent_node = parent_node;

        if (!parent_node) return;

        parent_node->childrens.push_back(this);
    
    }

    Node *Node::add(Node *node) {

        node->parent(this);

        return node;

    }

    void Node::remove(Node *child) {  

        auto it = std::find(childrens.begin(), childrens.end(), child);

        if ( std::distance(childrens.begin(), it) < 0 ) {
            
            PLOGD << "could not delete, didnt found";
            return; 
            
        }

        childrens.erase(it);

    }

    uint32_t Node::index() { 

        auto it = std::find(parent_node->childrens.begin(), parent_node->childrens.end(), this);

        return std::distance(parent_node->childrens.begin(), it);

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

