#include "tree.hpp"
#include "node.hpp"
#include "engine.hpp"
#include "imgui_internal.h"

TreeWidget::TreeWidget(Node* selected) : GUI::Window("Tree"), selected(selected) {  }

void TreeWidget::draw()  { 

    if (!selected) selected = &engine.tree;
    
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
        
        drawChildrens(selected); 

        ImGui::EndTable();
    }
    
}

void TreeWidget::drawChildrens(Node* node) { 

    for (auto child : node->childrens) drawNode(child);

}

 
namespace ImGui {

bool TreeViewNode(Node* node) {

    ImGuiTreeNodeFlags flags =  ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow ;

    if (!node->childrens.size()) flags |= ImGuiTreeNodeFlags_Leaf;

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return 0;

    // Layout
    // const ImVec2 curr_pos = ImVec2(0,GetCursorPos().y);
    const ImVec2 curr_pos(window->DC.CursorPos.x, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);

    const char *badbad = "mmmmmmmmmmmmmmmmmmmmmmmmmm";
    const ImVec2 text_size = CalcTextSize(badbad,badbad+15);
    ImRect bb(curr_pos.x, curr_pos.y, curr_pos.x + text_size.x, curr_pos.y + text_size.y);
    ItemSize(text_size, 0.0f);
    if (!ItemAdd(bb, 0))
        return 0;

    auto hovered = IsItemHovered();

    if(engine.selected != node) {


        if (engine.selected) { 

            auto c = node->color;

            const ImVec4 imc = ImVec4(.75, .75, .75, .75);  // shoud use node->color; !!

            ImGui::PushStyleColor(ImGuiCol_Text, imc);

        
        }

        if (hovered) PushStyleColor(ImGuiCol_Text, ImVec4(1, .4, 0, 1));

    }

    SameLine();
    SetCursorPosX(GetCursorPosX()-text_size.x);
    auto x = TreeNodeEx((node->name).c_str(), flags);
    
    if(engine.selected != node) {

        if (hovered) PopStyleColor();

        if (engine.selected) ImGui::PopStyleColor(1);

    }

    return x;

}

};
void TreeWidget::drawNode(Node* node) { 
    
    ImGui::TableNextRow();

    if (ImGui::TableNextColumn()) {

        ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();

        const bool recurse = ImGui::TreeViewNode(node);

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

        if(ImGui::MenuItem("zoom")) engine.gui->trees[0]->selected = node;
      
        if(ImGui::MenuItem("pop")) engine.gui->trees.push_back(new TreeWidget(node));

        ImGui::EndPopup();
    }
    // if(!ImGui::IsPopupOpen("#popup")){is_deleting = false;}
     
        if (ImGui::BeginDragDropSource()) {

            auto ptr = (uint64_t)node;
            ImGui::SetDragDropPayload("_TREENONODE", &(ptr), sizeof(uint64_t));
            ImGui::Text(node->name.c_str());
            ImGui::EndDragDropSource();
            
        }else {
            
            if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0)) { Engine::getInstance().selected = node; }

        }

        if (ImGui::BeginDragDropTarget()) {     

            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENONODE")) node->add((Node*)(*(uint64_t*)payload->Data));

            ImGui::EndDragDropTarget();
            
        }

        
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