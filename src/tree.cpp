#include "tree.hpp"
#include "node.hpp"
#include "engine.hpp"

Tree::Tree() : Node("tree"), GUI::Window("Tree")  {  }

void Tree::draw()  { 
    
    if (ImGui::BeginMenuBar()) {
        
        if (ImGui::BeginMenu("new")) {  

            if (ImGui::MenuItem("node")) {

                Engine::getInstance().tree.add(new Node());

            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();

    }

    drawNode(&Engine::getInstance().tree); 

}

void Tree::drawNode(Node* node) { 
    
ImGui::SetNextItemOpen(true);

if (ImGui::TreeNode(node->name.c_str())) {

    if (ImGui::BeginDragDropSource()) {

        auto ptr = (uint64_t)node;
        ImGui::SetDragDropPayload("_TREENODE", &ptr, sizeof(uint64_t));
        ImGui::Text(node->name.c_str());
        ImGui::EndDragDropSource();
        
    }

    if (ImGui::BeginDragDropTarget()) {     

        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE")) {
            
            Node* snode = (Node*)(*(uint64_t*)payload->Data);

            node->add(snode);

            PLOGD << snode->name << "(" << typeid(*snode).name() << ") > " << node->name << "(" << typeid(*node).name() << ")";

        }

        ImGui::EndDragDropTarget();
        
    }

    for (auto child : node->childrens) drawNode(child);
    ImGui::TreePop();

    }

}
