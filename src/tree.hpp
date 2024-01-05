#pragma once
#include "gui.hpp"
#include "node.hpp"


    struct TreeWidget : GUI::Window {

    TreeWidget() : GUI::Window("Tree")  {  }

    void draw() override { drawNode(&Engine::getInstance().tree); }

    void drawNode(Node* node) { 

            ImGui::SetNextItemOpen(true);

            if (ImGui::TreeNode(node->name.c_str())) {

                if (ImGui::BeginDragDropSource()) {

                    auto ptr = (uint64_t)node;
                    ImGui::SetDragDropPayload("_TREENODE", &ptr, sizeof(uint64_t));
                    ImGui::Text("This is a drag and drop source");
                    ImGui::EndDragDropSource();
                    
                }

                if (ImGui::BeginDragDropTarget()) {

                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE")) {
                        
                        Node* snode = (Node*)(*(uint64_t*)payload->Data);

                        snode->parent(node);

                        PLOGD << snode->name << " > " << node->name;
       
                    }

                     ImGui::EndDragDropTarget();
                    
                }
                

                for (auto child : node->childrens) drawNode(child);
                ImGui::TreePop();

                }
    

                
                
            }
    };