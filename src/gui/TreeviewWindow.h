#ifndef TREEVIEWWINDOW_H
#define TREEVIEWWINDOW_H

#pragma once
#include "gui.hpp"

    struct AnyNode {    

        const char * label = "null";    

        // Image icon;  

        ImVec4 color = {.1,.1,.1,.4};

        virtual bool drawNode() {                    
        
            // ImGui::TableNextRow();
            // ImGui::TableNextColumn();
            // ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(color));
            return ImGui::TreeNode(label); 
        
        }

        void draw() { drawNode(); }

    };

    template <typename T>
    struct Node : AnyNode {

        static inline std::vector<T*> pool; 
	
    	Node(const char * label) { pool.push_back((T*)this); }  
    
    };

    struct GroupNode : public AnyNode {

        std::vector<AnyNode*> tree;
    
        template <typename T>
        T* addNode(T* node) { tree.push_back(node); return (T*)tree.back(); }

        virtual void drawTree() { for (AnyNode* child : tree) child->draw(); }

        bool drawNode() override {   
            
            ImGui::SetNextItemOpen(true, ImGuiCond_Once); // always open

            // if (ImGui::IsItemClicked()) current = this;

            // ImGui::TableNextRow();
            // ImGui::TableNextColumn();

            // ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(color));

            // ImU32 cell_bg_color = ImGui::GetColorU32(color);
            // ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, cell_bg_color);
          

            bool tn = AnyNode::drawNode();
            
            // ImGui::TableNextColumn();
  
            // ImGui::Checkbox("", &tn);
            // ImGui::SameLine();
            // ImGui::SmallButton("+");

            if (tn) {

                drawTree();
                ImGui::TreePop();

            }

            return tn;

        }

    };

    template <typename T>
    struct PoolNode : public GroupNode {

	    void drawTree() override { for (auto* child : T::pool) child->drawNode(); }

    };


struct TreeviewWindow : GUIRenderer, GroupNode {

    template <typename T>
    AnyNode* addPool(const char * label) { 
        
        auto n = new PoolNode<T>();
        n->label = label;
        return addNode(n); 
    
    }

    void draw() { 

        ImGui::Begin("Tree", NULL, ImGuiWindowFlags_NoCollapse);

        // if (ImGui::BeginTable("3ways", 2, ImGuiTableFlags_RowBg)) {   
           
        //     ImGui::TableSetupColumn("Name");
        //     ImGui::TableSetupColumn("On/Off");
        //     ImGui::TableHeadersRow();

            GroupNode::drawTree();

        //     ImGui::EndTable();
        
        // }

        ImGui::End();

    }

    // ~TreeviewWindow();

};

#endif