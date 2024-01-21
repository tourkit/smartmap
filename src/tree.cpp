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
        if (ImGui::BeginMenu(std::to_string(ImGui::GetIO().Framerate).c_str())) {  
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();

    }

    // Create the table
    if (ImGui::BeginTable("TreeTable", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
        
        drawChildrens(&Engine::getInstance().tree); 

        ImGui::EndTable();
    }

    ImGui::SliderInt4("blank", (int*)&Engine::getInstance().blank[0], -100, 100);
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

        // if (!node->childrens.size()) 
        flags |= ImGuiTreeNodeFlags_Leaf;

        ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();
        
        const bool recurse = ImGui::TreeNodeEx(node->name.c_str(), flags);
        
        if (ImGui::BeginDragDropSource()) {

            auto ptr = (uint64_t)node;
            ImGui::SetDragDropPayload("_TREENODE", &(ptr), sizeof(uint64_t));
            ImGui::Text(node->name.c_str());
            ImGui::EndDragDropSource();
            
        }

        if (ImGui::BeginDragDropTarget()) {     

            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE")) node->add((Node*)(*(uint64_t*)payload->Data));

            ImGui::EndDragDropTarget();
            
        }
            
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) { Engine::getInstance().editorw.selected = node; }
        
        ImGui::SameLine(pos);
        if (ImGui::Button("x")) delete node;
        
        const ImRect nodeRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

        if (recurse) {


            ImDrawList* drawList = ImGui::GetWindowDrawList();
            verticalLineStart.x+=7;
            verticalLineStart.y+=-7;
            ImVec2 verticalLineEnd = verticalLineStart;
            
            verticalLineEnd.y+=14;
            ImVec2 verticalLineEnd2 = verticalLineEnd;
            verticalLineEnd2.x+=10;//Engine::getInstance().blank[8];
            drawList->AddLine(verticalLineStart, verticalLineEnd, IM_COL32(255,255,255,255));
            drawList->AddLine(verticalLineEnd, verticalLineEnd2, IM_COL32(255,255,255,255));

            drawChildrens(node);

            ImGui::TreePop();

        }else {
            
            // if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) { Engine::getInstance().editorw.selected = node; }
        }
        
    
    }

}