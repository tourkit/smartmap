#include "tree.hpp"
#include "imgui.h"
#include "node.hpp"
#include "editor.hpp"
#include "window.hpp"
#include "imgui_internal.h"

TreeWidget::TreeWidget(GUI* gui) : GUI::Window("Tree", gui) {

    memset( &search_str[0], 0, sizeof(search_str) );

    strncpy( &search_str[0], &filter_str[0], sizeof(filter_str) );

 }




static bool demodemo = false;

void TreeWidget::draw()  {
 
if (demodemo) ImGui::ShowDemoWindow();


    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,ImVec2(4,1));

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 26);
    

    
    ImGui::SetCursorPosX(5);
    ImGui::InputText("###filtersearch", &search_str[0], sizeof(search_str));


    if (ImGui::IsItemActivated()) {

        memset( &search_str[0], 0, sizeof(search_str) );

        // c bo
        if(ImGuiInputTextState* state { ImGui::GetInputTextState(ImGui::GetItemID()) })
            state->ClearText();
    
        filtering  = true;        

    }

    if (ImGui::IsItemDeactivated()) {

        memset( &search_str[0], 0, sizeof(search_str) );

        strncpy( &search_str[0], &filter_str[0], sizeof(filter_str) );

        filtering = false;
    
    }

    if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftShift)) && ImGui::IsKeyDown(ImGuiKey_Slash)) {
    
        ImGui::SetKeyboardFocusHere(-1);
    
    }
    
    if (ImGui::IsKeyDown(ImGuiKey_Escape)) {

        filtering = false;
    
    }


    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX()-6);
    if (ImGui::Button("+")) gui->editors.push_back(new EditorWidget(gui));


    ImGui::PopItemWidth();

    ImGui::BeginChild("envoisinduvenin");

    // Create the table

    if (ImGui::BeginTable("TreeTable", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {

        if (selected)
            drawChildrens(selected);

        ImGui::EndTable();
    }

    ImGui::EndChild();

    ImGui::PopStyleVar(1);
}

void TreeWidget::drawChildrens(Node* node) { for (auto child : node->childrens) drawNode(child); }



bool TreeWidget::TreeViewNode(Node* node) {
using namespace ImGui;


    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick ;

    if (!node->childrens.size()) flags |= ImGuiTreeNodeFlags_Leaf;
    if (node->open) flags |= ImGuiTreeNodeFlags_DefaultOpen;

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return 0;

    const ImVec2 curr_pos = ImVec2(0,GetCursorPos().y);

    auto t_pos = GetCursorPosX();

    SetCursorPosX(GetWindowWidth()-19);

    std::string str = "##active"+std::to_string(node->uid);
    Checkbox(str.c_str(), &node->is_active);
    SameLine();
    SetCursorPosX(t_pos);

    bool hovered = false;

    if (ImGui::GetMousePos().y > curr_pos.y+ImGui::GetWindowPos().y && ImGui::GetMousePos().y < curr_pos.y+ImGui::GetWindowPos().y+ImGui::GetTextLineHeight()) hovered = true;


    ImVec4 node_color = *(ImVec4*)&node->color;

    if(selected != node) {

        if (hovered) node_color = ImVec4(1, .4, 0, 1);

        else if (selected) node_color.w = .65;

    }

    ImGui::PushStyleColor(ImGuiCol_Text, node_color);

    SetCursorPosX(GetCursorPosX()-1);
    bool x = false;

    if (is_renaming != node) {
        
        x = TreeNodeEx(node->name().c_str(), flags);

        if (ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(0)) {
            is_renaming = node;
            memset(&renaming_name[0],0,612);
            memcpy(&renaming_name[0], node->name().c_str(), node->name().length());
        }

    } else {

        if (ImGui::InputText("##jksdhfjksdfjk", &renaming_name[0], 512, ImGuiInputTextFlags_EnterReturnsTrue)) {

            gui->rename_list[node] = &renaming_name[0];

            is_renaming = nullptr;

        }
    }

    if (ImGui::BeginPopupContextItem()) {

        if (ImGui::BeginMenu("add")) {

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
                gui->delete_list.push_back(node);

                gui->window->end_of_render_cbs.emplace_back(std::pair<void*,std::shared_ptr<std::function<void(void*)>>>{nullptr, std::make_shared<std::function<void(void*)>>([&](void* ptr) { 

                    delete (Node*)ptr;

                })});

            }

        }

        if (!ImGui::IsItemHovered()) 
            is_deleting = false;

        if(ImGui::MenuItem("rename")) {

            is_renaming = node;

            memset(&renaming_name[0],0,612);
            memcpy(&renaming_name[0], node->name().c_str(), node->name().length());

        }

        if(ImGui::MenuItem("update")) 
            node->update();

        if (ImGui::BeginMenu("trig")) {
            
            if (ImGui::MenuItem("CHANGE")) node->trig(Node::CHANGE);
            if (ImGui::MenuItem("CREATE")) node->trig(Node::CREATE);

            ImGui::EndMenu();
        }

        if(ImGui::MenuItem("zoom")) 
            gui->trees[0]->selected = node;

        if(ImGui::MenuItem("pop")) {
            gui->trees.push_back(new TreeWidget(gui));
            gui->trees.back()->selected = node;
        }

        if (ImGui::MenuItem("editor")) {

            gui->editors.push_back(new EditorWidget(gui));
            gui->editors.back()->selected = node;
            gui->editors.back()->locked = true;

        }

        ImGui::Checkbox("demo", &demodemo);

        Separator();

        ImGui::Text(node->type_name().c_str());

        ImGui::EndPopup();
    }

    PopStyleColor();

    static bool holding = false;

    if (ImGui::BeginDragDropSource()) {

        auto ptr = (uint64_t)node;
        ImGui::SetDragDropPayload("_TREENONODE", &(ptr), sizeof(uint64_t));

        holding = true;

        ImGui::Text(node->name().c_str());
        ImGui::EndDragDropSource();

    }else {

        static bool mouse_down = false;

        static Node* s = nullptr;

        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) { mouse_down = true; s = node; }

        if (mouse_down) 
            if (ImGui::IsMouseReleased(0) && !holding) {
                
                gui->selected = s;

                if (!gui->editors.size()) 
                    gui->editors.emplace_back(new EditorWidget(gui));
                
            }

        if (ImGui::IsMouseReleased(0)) mouse_down = false;

    }

    if (ImGui::BeginDragDropTarget()) {

        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENONODE")) {

            auto n = (Node*)(*(uint64_t*)payload->Data);

            node->add(n);

            if (holding) 
                holding = false;

        }

        ImGui::EndDragDropTarget();

    }



    return x;

}


void TreeWidget::drawNode(Node* node) {

    if (node->hidden) return;

    std::regex pattern; bool pattern_error = false;
    try { pattern.assign(&search_str[0], std::regex_constants::icase); }
    catch (const std::regex_error& e) {  pattern_error = true; }

    if (!filtering || !strlen(&search_str[0]) || (!pattern_error && std::regex_search(node->name().c_str(), pattern))) {

        ImGui::TableNextRow();

        if (ImGui::TableNextColumn()) {

            ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();

            bool recurse = TreeViewNode(node);

        // if(!ImGui::IsPopupOpen("#popup")){is_deleting = false;}

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

                if (ImGui::IsDragDropActive()) {

                    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding,ImVec2(4,0));
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,ImVec2(4,0));

                    ImVec2 dropperline;
                    dropperline.x = ImGui::GetWindowPos().x;
                    dropperline.y = ImGui::GetCursorScreenPos().y;
                    dropperline.y += 3;

                    ImVec2 dropperline2 = dropperline;
                    dropperline2.x += ImGui::GetWindowWidth();
                    dropperline2.y -= 4;

                    if (ImGui::BeginDragDropTarget()) {

                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENONODE")) 
                            {PLOGI << "TODO MOVE NODE";}

                        ImGui::EndDragDropTarget();
                    }
                    ImGui::SameLine();
                    ImGui::PushID(6969);
                    ImGui::BeginGroup();
                    drawList->AddRectFilled(dropperline, dropperline2, IM_COL32(255,255,0,40));
                    ImGui::EndGroup();
                    ImGui::PopID();


                    ImGui::PopStyleVar(2);

                }

                drawChildrens(node);

                ImGui::TreePop();

            }

        }

    }else
        if (filtering && search_str[0]) 
            drawChildrens(node); 

}
