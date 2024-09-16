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





void TreeWidget::draw()  {
 
    if (gui->show_demo) ImGui::ShowDemoWindow();


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

    static bool slashdown = false;

    if (!slashdown && ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyDown(ImGuiKey_Slash)) {

        slashdown = true;

        if (ImGui::IsItemFocused())
            ImGui::SetKeyboardFocusHere(1);
        else 
            ImGui::SetKeyboardFocusHere(-1);
        
    }
    
    if (slashdown && ImGui::IsKeyReleased(ImGuiKey_Slash)) 
            slashdown = false;
    
    if (ImGui::IsKeyDown(ImGuiKey_Escape)) {

        filtering = false;
    
    }


    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX()-6);
    if (ImGui::Button("+")) gui->editors.push_back(new EditorWidget(gui));


    ImGui::PopItemWidth();

    ImGui::BeginChild("envoisinduvenin");



    
    try { pattern.assign(&search_str[0], std::regex_constants::icase); }
    catch (const std::regex_error& e) {  pattern_error = true; }


    // Create the table

    if (ImGui::BeginTable("TreeTable", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {

        if (selected)
            for (auto child : selected->childrens) 
                TreeViewNode(child);

        ImGui::EndTable();
    }

    ImGui::EndChild();

    ImGui::PopStyleVar(1);
}





bool TreeWidget::TreeViewNode(Node* node, int depth) {

    ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();
    
    using namespace ImGui;


    if (!filtering || !strlen(&search_str[0]) || (!pattern_error && std::regex_search(node->name().c_str(), pattern))) {
    ImVec2 t_pos = GetCursorPos();
    t_pos.x = depth*8;

    ImGui::TableNextRow();

    if (ImGui::TableNextColumn()) {



        ////////////////////
        // checkbox
        ////////////////////

        SetCursorPosX(GetWindowWidth()-19);

        Checkbox(("##active"+std::to_string(node->uid)).c_str(), &node->is_active);

        SameLine();
        
        ////////////////////
        // + for open 
        ////////////////////

        if (node->childrens.size()) {    

            SetCursorPosX(t_pos.x+10);

            Text("+");

            if (IsItemClicked(0)) 
                node->open = !node->open;
            
            SameLine();

        }

        ImGui::SetCursorPosX(t_pos.x+21);

        ////////////////////
        // connection lines
        ////////////////////

        const ImRect nodeRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        static TestWin testwin("test", gui);
        testwin.floats.resize(10);

        verticalLineStart.x+=(depth*8)+testwin.floats[0];//21
        verticalLineStart.y+=-10+testwin.floats[1];
        ImVec2 verticalLineEnd = verticalLineStart;

        verticalLineEnd.y+=18+testwin.floats[2];
        ImVec2 verticalLineEnd2 = verticalLineEnd;
        verticalLineEnd2.x+=12+testwin.floats[3];//Engine::getInstance().blank[8];

        if (ImGui::GetCursorPosX() > 25+testwin.floats[4]) {
            drawList->AddLine(verticalLineStart, verticalLineEnd, IM_COL32(200,200,200,50));
            
            verticalLineEnd.x = std::max(verticalLineEnd.x+10, ImGui::GetWindowPos().x+20+testwin.floats[5])-10;
            drawList->AddLine(verticalLineEnd, verticalLineEnd2, IM_COL32(200,200,200,50));

        }


        ////////////////////
        //draw name
        ////////////////////

        bool hovered = false;

        if (ImGui::GetMousePos().y > t_pos.y+ImGui::GetWindowPos().y && ImGui::GetMousePos().y < t_pos.y+ImGui::GetWindowPos().y+ImGui::GetTextLineHeight()) hovered = true;

        ImVec4 node_color = *(ImVec4*)&node->color;

        if(gui->selected != node) {

            if (hovered) node_color = ImVec4(1, .4, 0, 1);

            else node_color.w = .65;

        }

        if(node->error) 
            node_color = ImVec4(1, 0, 0, 1);


        if (is_renaming != node) {
        ImGui::PushStyleColor(ImGuiCol_Text, node_color);
            
            ImGui::SetCursorPosX(t_pos.x+21);
            Text(node->name().c_str());



            if (ImGui::IsItemClicked()){
                if (ImGui::IsMouseDoubleClicked(0)) {
                    is_renaming = node;
                    memset(&renaming_name[0],0,612);
                    memcpy(&renaming_name[0], node->name().c_str(), node->name().length());
                }else{
                    if (IsMouseDown(0))
                        gui->selected = node;
                    // else if (IsMouseDown(1))
               }
            }

            PopStyleColor();




        } else {

            SetCursorPosX(t_pos.x + 17 );
            
            if (ImGui::InputText("##jksdhfjksdfjk", &renaming_name[0], 512, ImGuiInputTextFlags_EnterReturnsTrue)) {

                gui->rename_list[node] = &renaming_name[0];

                is_renaming = nullptr;

            }
        }


    }


                //     if (ImGui::BeginPopupContextItem()) {

                // bool will_exit = false;


                // ImGui::EndPopup();
            // }
}
    if (node->open || filtering)
        for (auto child : node->childrens) 
            TreeViewNode(child, depth+1); 

    return true;

}

    
    // if (!filtering || !strlen(&search_str[0]) || (!pattern_error && std::regex_search(node->name().c_str(), pattern))) {

