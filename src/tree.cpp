#include "tree.hpp"
#include "engine.hpp"
#include "imgui.h"
#include "log.hpp"
#include "node.hpp"
#include "editor.hpp"
#include "window.hpp"
#include "imgui_internal.h"
#include <cstring>
#include <string>

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
    auto xxx = ImGui::GetInputTextState(ImGui::GetItemID());


    static int curr = 0;
    if (ImGui::IsItemDeactivated() && search_str[0] == '\0') {

        filtering = false;
        curr = 0;

        strncpy( &search_str[0], &filter_str[0], sizeof(filter_str) );

    }
        
    static bool slashdown = false;

    if (filtering && ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_Tab) && !ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ) {

        ImGui::SetKeyboardFocusHere(1);

        if (ImGui::IsItemFocused()){
            if (visible_list.size())
                gui->selected = visible_list[0];

        }else 

            for (int i = 0; i < visible_list.size(); i++) 
                if (visible_list[i] == gui->selected) {
                    if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
                        gui->selected = visible_list[(i-1+visible_list.size())%visible_list.size()];
                    else
                        gui->selected = visible_list[(i+1)%visible_list.size()];
                    break;
                    
                }
        
        if (gui->editors.size())
            gui->editors[0]->selected = gui->selected;
    }

    if (!slashdown && ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_Slash)) {

        slashdown = true;

        if (ImGui::IsItemFocused()){

            memset( &search_str[0], 0, sizeof(search_str) );
        
            ImGui::SetKeyboardFocusHere(1);

        }
        else {

            ImGui::SetKeyboardFocusHere(-1);

            if (!strcmp(&search_str[0], &filter_str[0]))
                memset( &search_str[0], 0, sizeof(search_str) );

            if(ImGuiInputTextState* state { ImGui::GetInputTextState(ImGui::GetItemID()) })
                state->ClearText();
        
            filtering  = true;        

        }
        
    }
    
    if (slashdown && ImGui::IsKeyReleased(ImGuiKey_Slash)) 
            slashdown = false;
    
    if (ImGui::IsItemActivated() && !strcmp(&xxx->TextA[0], &filter_str[0])) {

        memset( &search_str[0], 0, sizeof(search_str) );
        xxx->ClearText();
        filtering  = true;     

    }

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX()-6);
    if (ImGui::Button("+")) 
        gui->window->end_of_render_cbs.push_back(std::pair<void*, std::function<void(void*)>>{nullptr, ([&](void*) { 
            gui->editors.push_back(new EditorWidget(gui)); 
        })});


    ImGui::PopItemWidth();

    ImGui::BeginChild("envoisinduvenin");



    
    try { pattern.assign(&search_str[0], std::regex_constants::icase); }
    catch (const std::regex_error& e) {  pattern_error = true; }


    // Create the table

    if (ImGui::BeginTable("TreeTable", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {

        visible_list.clear();
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

    if (node->hidden) 
        return false;
    if (!filtering || !strlen(&search_str[0]) || (!pattern_error && std::regex_search(node->name().c_str(), pattern))) {
   
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        ImVec2 t_pos = GetCursorPos();

        t_pos.x = depth*8;

        ImGui::TableNextRow();

        if (ImGui::TableNextColumn()) {

            ////////////////////
            // checkbox
            ////////////////////

            SetCursorPosX(GetWindowWidth()-19);

            if (Checkbox(("##active"+std::to_string(node->uid)).c_str(), &node->is_active)) 
                node->active(node->is_active);
            

            SameLine();
            
            ////////////////////
            // + for open 
            ////////////////////

            // static TestWin testwin("test", gui);
            // testwin.floats.resize(10);

            bool has_vis = false;
            for (auto x : node->childrens) 
                if (!x->hidden){
                    has_vis = true;
                    break;
                }

            if (has_vis) {    

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

            verticalLineStart.x+=(depth*-2);//21
            verticalLineStart.y+=-10;
            ImVec2 verticalLineEnd = verticalLineStart;

            verticalLineEnd.y+=18;
            ImVec2 verticalLineEnd2 = verticalLineEnd;
            verticalLineEnd2.x+=12;//Engine::getInstance().blank[8];

            if (ImGui::GetCursorPosX() > 25) {
                drawList->AddLine(verticalLineStart, verticalLineEnd, IM_COL32(200,200,200,50));
                
                verticalLineEnd.x = std::max(verticalLineEnd.x+10, ImGui::GetWindowPos().x+20)-10;
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
                        
                /////////////////
                // pop up right clik
                /////////////////

                if (BeginPopupContextItem((std::to_string(gui->member_count++)).c_str())) {
                    
                    Separator();

                    if (!is_deleting) {

                        ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
                        if(ImGui::MenuItem("delete")){ is_deleting = true; }
                        ImGui::PopItemFlag();

                    }else if(ImGui::MenuItem("Sure ?")){

                        is_deleting = false;
                        
                    this->gui->deleteNode(node);

                    }

                    if (!ImGui::IsItemHovered()) 
                        is_deleting = false;
                    
                    Separator();

                    if (ImGui::BeginMenu("trig")) {
                        Separator();
                        if (ImGui::MenuItem("CHANGE")) node->trig(Node::CHANGE);
                        Separator();
                        if (ImGui::MenuItem("CREATE")) node->trig(Node::CREATE);
                        Separator();
                        ImGui::EndMenu();
                    }

                    Separator();

                    ImGui::MenuItem(node->type_name().c_str());
                    
                    Separator();

                    EndPopup();
                }


                if (filtering)
                    visible_list.push_back(node);

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

                if (IsMouseReleased(0) && gui->selected){
                    if (gui && !gui->editors.size()){
                        gui->editors.emplace_back(new EditorWidget(gui));
                        gui->editors.back()->selected = node;
                    }
                }

                PopStyleColor();

            } else {

                

                SetCursorPosX(t_pos.x + 17 );
                
                if (ImGui::InputText("##jksdhfjksdfjk", &renaming_name[0], 512, ImGuiInputTextFlags_EnterReturnsTrue)) {

                    gui->window->end_of_render_cbs.emplace_back(std::pair<void*,std::function<void(void*)>>({node, [](void* ptr){ 
                        logger.cout(Sev::verbose);
                        ((Node*)ptr)->name(((Node*)ptr)->name_v); 
                        logger.cout(Sev::warning);
                    }}));
                    node->name_v = &renaming_name[0];

                    is_renaming = nullptr;

                }
            }

        }



        //////////////////
        // drag n drop
        //////////////////

        static bool holding = false;

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {

            auto ptr = (uint64_t)node;
            ImGui::SetDragDropPayload("_TREENONODE", &(ptr), sizeof(uint64_t));

            holding = true;

            ImGui::Text(node->name().c_str());
            ImGui::EndDragDropSource();

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

    }


    TreePush(("###pop"+std::to_string(depth)).c_str());
    if (node->open || filtering)
        for (auto child : node->childrens) {
            TreeViewNode(child, depth+1); 
            
        }
    TreePop();

    return true;

}


