#include "node.hpp"
#include "file.hpp"
#include "log.hpp"
#include "engine.hpp"


    UntypedNode::UntypedNode(std::string name, glm::vec4 color) : name(name), color(color) {

        PLOGV << name << " - " << type().name();
        
    }

    UntypedNode::~UntypedNode() {

        auto t_childrens = childrens;
        for (auto c : t_childrens) delete c;
        t_childrens.resize(0);

        if (parent_node) parent_node->remove(this);

        if (dtor) dtor(parent_node);

        PLOGV << "~" << name;

    }   

    Node* UntypedNode::add(void* node_v)  {
        
        auto n = (UntypedNode*)node_v; 

         n->parent(this);   

        return (Node*)n;

     }


    void UntypedNode::import(std::string path) {

        File file(path);

        import(&file);

    }

    UntypedNode *UntypedNode::parent() { return parent_node; }

    void UntypedNode::select(){ engine.selected = this; }
    
    void UntypedNode::parent(UntypedNode* parent_node) {  
        
        if (this->parent_node == parent_node) return;

        if (this->parent_node) this->parent_node->remove(this);
        
        this->parent_node = parent_node;

        if (!parent_node) return;

        parent_node->childrens.push_back(this);
    
    }

    void UntypedNode::update() { if (parent_node) parent_node->update(); }

    void UntypedNode::remove(UntypedNode *child) {  

        auto it = std::find(childrens.begin(), childrens.end(), child);

        if ( std::distance(childrens.begin(), it) < 0 ) {
            
            PLOGD << "could not delete, didnt found";
            return; 
            
        }

        childrens.erase(it);

        update();

    }

    uint32_t UntypedNode::index() { 

        auto it = std::find(parent_node->childrens.begin(), parent_node->childrens.end(), this);

        return std::distance(parent_node->childrens.begin(), it);

    }

    void UntypedNode::runCB() { }

    void UntypedNode::run() { 

        for (auto c : childrens) c->run();

        runCB();
        
    }

    void UntypedNode::up() { 
        
        if (!parent_node) return;

        auto it = std::find(parent_node->childrens.begin(), parent_node->childrens.end(), this);
        int index = std::distance(parent_node->childrens.begin(), it);

        if(index<1) return; 

        parent_node->childrens.erase(it);
        parent_node->childrens.insert(parent_node->childrens.begin() + index - 1, this);
    
    }

    void UntypedNode::down() {

        if (!parent_node) return;
        
        auto it = std::find(parent_node->childrens.begin(), parent_node->childrens.end(), this);
        int index = std::distance(parent_node->childrens.begin(), it);  

        if(index > parent_node->childrens.size()-2) return;

        parent_node->childrens.erase(it);
        parent_node->childrens.insert(parent_node->childrens.begin() + index + 1, this); 

    }
