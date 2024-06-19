#include "tree.hpp"
#include "node.hpp"
#include "engine.hpp"
#include "drawcall.hpp"
#include "imgui_internal.h"

TreeWidget::TreeWidget(Node* selected) : GUI::Window("Tree"), selected(selected) {

    memset( &search_str[0], 0, sizeof(search_str) );

    strncpy( &search_str[0], &filter_str[0], sizeof(filter_str) );

 }




static bool demodemo = false;

void TreeWidget::draw()  {

if (demodemo) ImGui::ShowDemoWindow();




  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,ImVec2(4,1));


    if (!selected) selected = engine.tree;

    if (selected != engine.tree) name = selected->name();
    else name = "Tree";

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 24);
    ImGui::InputText("###filtersearch", &search_str[0], sizeof(search_str), ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SameLine();

    if (ImGui::IsItemHovered()) {

        if (!strcmp(&search_str[0], &filter_str[0])) {

            memset( &search_str[0], 0, sizeof(filter_str) );

            filtering = true;

        }

    }else{

        if (!strlen(search_str)) {

             memset( &search_str[0], 0, sizeof(search_str) );

            strncpy( &search_str[0], &filter_str[0], sizeof(filter_str) );

            filtering = false;

        }

    }
    if (ImGui::Button("+")) engine.gui->editors.push_back(new EditorWidget());


    ImGui::PopItemWidth();

    ImGui::BeginChild("envoisinduvenin");

    // Create the table

    if (ImGui::BeginTable("TreeTable", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {

        drawChildrens(selected);

        for (auto x : delete_list) {

            auto parent = x->parent();

            delete x;

            if (parent) parent->update();
        }

        delete_list.clear();

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

    SetCursorPosX(GetWindowWidth()-30);

    std::string str = "##active"+std::to_string(node->uid);
    Checkbox(str.c_str(), &node->is_active);
    SameLine();
    SetCursorPosX(t_pos);

    bool hovered = false;

    if (ImGui::GetMousePos().y > curr_pos.y+ImGui::GetWindowPos().y && ImGui::GetMousePos().y < curr_pos.y+ImGui::GetWindowPos().y+ImGui::GetTextLineHeight()) hovered = true;


    ImVec4 node_color = *(ImVec4*)&node->color;

    if(engine.selected != node) {

        if (hovered) node_color = ImVec4(1, .4, 0, 1);

        else if (engine.selected) node_color.w = .65;

    }

    ImGui::PushStyleColor(ImGuiCol_Text, node_color);

    SetCursorPosX(GetCursorPosX()-1);
    bool x = false;

    if (is_renaming != node) {x = TreeNodeEx(node->name().c_str(), flags);
        if (ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(0)) {is_renaming = node;
        memset(&renaming_name[0],0,612);
        memcpy(&renaming_name[0], node->name().c_str(), node->name().length());}
    }
    else {

PLOGE<<"coc";
        if (ImGui::InputText("##jksdhfjksdfjk", &renaming_name[0], 512, ImGuiInputTextFlags_EnterReturnsTrue)) {

            node->name(&renaming_name[0]);

            is_renaming = nullptr;

        }
    }
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
                delete_list.push_back(node);

            }

        }

        if (!ImGui::IsItemHovered()) is_deleting = false;

        if(ImGui::MenuItem("rename")) {

            is_renaming = node;

        memset(&renaming_name[0],0,612);
        memcpy(&renaming_name[0], node->name().c_str(), node->name().length());

        }

        if(ImGui::MenuItem("zoom")) engine.gui->trees[0]->selected = node;

        if(ImGui::MenuItem("pop")) engine.gui->trees.push_back(new TreeWidget(node));

        if (ImGui::MenuItem("editor")) {

            engine.gui->editors.push_back(new EditorWidget());
            engine.gui->editors.back()->selected = node;
            engine.gui->editors.back()->locked = true;

        }

        ImGui::Checkbox("demo", &demodemo);

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

        if (mouse_down) if (ImGui::IsMouseReleased(0) && !holding) Engine::getInstance().selected = s;

        if (ImGui::IsMouseReleased(0)) mouse_down = false;

    }

    if (ImGui::BeginDragDropTarget()) {

        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENONODE")) {

            auto n = (Node*)(*(uint64_t*)payload->Data);

            if (node->onaddtyped_cb[node->type()].find(n->type()) != node->onaddtyped_cb[node->type()].end() || node->onadd_cb.find(n->type()) != node->onadd_cb.end()) node->add(n);

            if (holding) holding = false;

        }

        ImGui::EndDragDropTarget();

    }



    return x;

}


void TreeWidget::drawNode(Node* node) {

    if (node->hidden) return;

    std::regex pattern; bool pattern_error = false;
    try { pattern.assign(search_str, std::regex_constants::icase); }
    catch (const std::regex_error& e) {  pattern_error = true; }

    if (!filtering || !strlen(search_str) || (!pattern_error && std::regex_search(node->name().c_str(), pattern))) {

        ImGui::TableNextRow();

        if (ImGui::TableNextColumn()) {

            ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();

            bool recurse = recurse = TreeViewNode(node);

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

                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENONODE"))PLOGI << "TODO MOVE NODE";

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

    }else{ if (filtering && search_str) drawChildrens(node); }

}
