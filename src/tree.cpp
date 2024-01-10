#include "tree.hpp"
#include "node.hpp"
#include "engine.hpp"

Tree::Tree() : Node("tree"), GUI::Window("Tree")  {  }

void Tree::draw()  { 
    
    if (ImGui::BeginMainMenuBar()) {
        
        if (ImGui::BeginMenu("new")) {  

            if (ImGui::MenuItem("node")) {

                Engine::getInstance().tree.add(new Node());

            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();

    }

    // Create the table
    if (ImGui::BeginTable("TreeTable", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
        
        drawChildrens(&Engine::getInstance().tree); 

        ImGui::EndTable();
    }
}

void Tree::drawChildrens(Node* node) { 

    for (auto child : node->childrens) drawNode(child);

}

void Tree::drawNode(Node* node) { 

    auto pos = ImGui::GetColumnWidth()-ImGui::GetColumnOffset();
    
    ImGui::TableNextRow();
    // ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0);

    if (ImGui::TableNextColumn()) {

        ImGuiTreeNodeFlags flags =  ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow ;

        if (!node->childrens.size()) flags |= ImGuiTreeNodeFlags_Leaf;

        if (ImGui::TreeNodeEx(node->name.c_str(), flags)) {

            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) { Engine::getInstance().editorw.selected = node; }
            ImGui::SameLine(pos);
            if (ImGui::Button("x")) {
                delete node;
            }
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

            drawChildrens(node);

            ImGui::TreePop();

        }else {
            
            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) { Engine::getInstance().editorw.selected = node; }
        }
        
    
    }

}