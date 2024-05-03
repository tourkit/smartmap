#include "node.hpp"
#include "log.hpp"
#include "engine.hpp"
#include "buffer.hpp"
#include "ubo.hpp"


    UntypedNode::UntypedNode(std::string name, ImVec4 color) : name_v(name), color(color) {

        uid = total_uid++;

        pool.insert(this);
        PLOGV << "#" << name;

    }

    std::string UntypedNode::name() { return name_v; }

    void UntypedNode::name(std::string value) { name_v = value;
    // onchange_cb(node());
     }

    UntypedNode::~UntypedNode() {

        auto t_childrens = childrens;
        for (auto c : childrens) delete c;

        t_childrens = hidden_childrens;
        for (auto c : t_childrens) delete c;

        for (auto x : pool) for (auto r : x->referings) if (r == this) { x->referings.erase(r); break; }


        if (ondelete_cb) ondelete_cb(this->node());

        if (parent_node) parent_node->remove(node());

        pool.erase(this);
        PLOGV << "~" << name();

    }

    Node* UntypedNode::child(std::vector<std::string> names) {

        auto traget = names.back();

        for (auto c : childrens) {

            if (!strcmp(names.back().c_str(), c->name().c_str())) {

                if (names.size()== 1) return c;

                auto parent = c;

                for (int i = names.size()-2; i >= 0; i--) if (!strcmp(parent->parent()->name().c_str(),names[i].c_str())) { parent = parent->parent(); }else{ c = nullptr; break; }

                if (c) return c;

            }

        }

        if (names.size()> 1) for (auto c : childrens) {

            auto x = c->child(names);

            if (x) return x;

        }

        return nullptr;

    }
    Node* UntypedNode::child(const char* name) {

        std::string input = name;
        std::vector<std::string> names;
        std::string delimiter = "::";
        size_t pos = 0;
        std::string token;
        while ((pos = input.find(delimiter)) != std::string::npos) {
            token = input.substr(0, pos);
            names.push_back(token);
            input.erase(0, pos + delimiter.length());
        }
        names.push_back(input);

        return child(names);

    }

    Node* UntypedNode::active(bool value) { is_active = value; return node(); }

    Node* UntypedNode::node() { return (Node*)this; }

    Node* UntypedNode::top() { auto top = node(); while(top->parent()) { top = top->parent(); } return top; }

    void UntypedNode::addList(NodeList *nodes) { for (auto n : *nodes) add(n); }

    Node* UntypedNode::add(void* node_v)  {

        auto n = (Node*)node_v;

        PLOGV << type_name() << "::" << name() << " add " << n->type_name() << "::" << n->name();

        if (n->parent() == node()) return nullptr;

        if (onadd_cb.find(n->type()) != onadd_cb.end()) {

            n = onadd_cb[n->type()](this->node(),n->node());

            if (!n) return nullptr;

        }

        if (n == node_v) n->parent(node());

        else n->referings.insert(this->node());

        update();

        return n;

    }

    std::string UntypedNode::namesdf(){ if (parent()) { return parent()->name() + "::" + name(); } return name(); }

    Node *UntypedNode::parent() { return parent_node; }

    Node* UntypedNode::select(){ engine.selected = node(); return node();  }

    void UntypedNode::parent(Node* parent_node) {

        if (this->parent_node == parent_node) return;

        if (this->parent_node) this->parent_node->remove(node());

        this->parent_node = parent_node;

        if (!parent_node) return;

        is_active = parent_node->is_active;

        parent_node->childrens.push_back(node());

    }

    Node* UntypedNode::ondelete(std::function<void(Node*)> cb) { ondelete_cb = cb; return node(); }
    Node* UntypedNode::onchange(std::function<void(Node*)> cb) { onchange_cb = cb; return node(); }
    Node* UntypedNode::onrun(std::function<void(Node*)> cb) { onrun_cb = cb; return node(); }

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

    void UntypedNode::bkpupdate() {

        //TOFIX
        // engine.static_ubo.bkp();
        // engine.dynamic_ubo.bkp();

        update();

    }

    Node* UntypedNode::operator[](int id) { return childrens[id]->node(); }

    void UntypedNode::update() {

        PLOGV << type_name() << "::" << name();

        if (onchange_cb) onchange_cb(node());

        if (parent_node) parent_node->update();

        if (referings.size() && *referings.begin()) for (auto x : referings) if (x) x->update();

    }

    bool UntypedNode::remove(Node *child) {

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

    uint32_t UntypedNode::index() {

        auto it = std::find(parent_node->childrens.begin(), parent_node->childrens.end(), this);

        return std::distance(parent_node->childrens.begin(), it);

    }

    void UntypedNode::run() {

        if (!is_active && this != engine.tree) return; // PAas fan  sde  && this != engine.tree

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
