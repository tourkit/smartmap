#include "tree.hpp"
#include "node.hpp"
#include "engine.hpp"
#include "imgui_internal.h"

Tree::Tree() : GUI::Window("Tree")  { }

void Tree::draw()  { 
    
    if (ImGui::BeginMainMenuBar()) {
        
        if (ImGui::BeginMenu("new")) {  

            if (ImGui::MenuItem("node")) {

                // Engine::getInstance().tree->add(new Node());

            }

            ImGui::EndMenu();
        }

        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - ImGui::GetStyle().ItemSpacing.x*3);
        ImGui::TextUnformatted(std::to_string((int)std::round(ImGui::GetIO().Framerate)).c_str());

        ImGui::EndMainMenuBar();

    }

    std::string search_str = "filter";
    ImGui::PushItemWidth(-1);
    ImGui::InputText("###filtersearch", &search_str[0], search_str.size(), ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::PopItemWidth();

    // Create the table
    
    if (ImGui::BeginTable("TreeTable", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
        
        drawChildrens(Engine::getInstance().tree); 

        ImGui::EndTable();
    }
    
}

void Tree::drawChildrens(Node* node) { 

    for (auto child : node->childrens) drawNode(child);

}

void Tree::drawNode(Node* node) { 
    
    ImGui::TableNextRow();

    if (ImGui::TableNextColumn()) {

        ImGuiTreeNodeFlags flags =  ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow ;

        if (!node->childrens.size()) flags |= ImGuiTreeNodeFlags_Leaf;

        ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();
        

        if (engine.editorw.selected != node) { 

            // ImGui::PushStyleColor(ImGuiCol_Text, node->color);

        
        }
        const bool recurse = ImGui::TreeNodeEx((node->name).c_str(), flags);
       if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
    {
        if (ImGui::BeginMenu("add"))
        {
            if (ImGui::MenuItem("New")) {}
            ImGui::EndMenu();
        }
        bool will_exit = false;
        if (!is_deleting) {
        
            ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
            if(ImGui::MenuItem("delete")){ is_deleting = true; }
            ImGui::PopItemFlag();
        
        }else {

            if(ImGui::MenuItem("Sure ?")){
                
                is_deleting = false;
                delete node;
                
            }

        }

        if (!ImGui::IsItemHovered()) is_deleting = false;

        ImGui::MenuItem("pop");
        ImGui::EndPopup();
    }
    // if(!ImGui::IsPopupOpen("#popup")){is_deleting = false;}
     
        if (ImGui::BeginDragDropSource()) {

            auto ptr = (uint64_t)node;
            ImGui::SetDragDropPayload("_TREENONODE", &(ptr), sizeof(uint64_t));
            ImGui::Text(node->name.c_str());
            ImGui::EndDragDropSource();
            
        }

        if (ImGui::BeginDragDropTarget()) {     

            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENONODE")) node->add((Node*)(*(uint64_t*)payload->Data));

            ImGui::EndDragDropTarget();
            
        }
            
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) { Engine::getInstance().editorw.selected = node; }
        
        const ImRect nodeRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

        if (recurse) {


            ImDrawList* drawList = ImGui::GetWindowDrawList();
            verticalLineStart.x+=7;
            verticalLineStart.y+=-7;
            ImVec2 verticalLineEnd = verticalLineStart;
            
            verticalLineEnd.y+=14;
            ImVec2 verticalLineEnd2 = verticalLineEnd;
            verticalLineEnd2.x+=10;//Engine::getInstance().blank[8];
            drawList->AddLine(verticalLineStart, verticalLineEnd, IM_COL32(122,122,122,122));
            drawList->AddLine(verticalLineEnd, verticalLineEnd2, IM_COL32(122,122,122,122));

            drawChildrens(node);

            ImGui::TreePop();

        }else {
            
            // if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) { Engine::getInstance().editorw.selected = node; }
        }
        
    
    }

}