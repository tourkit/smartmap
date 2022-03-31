#ifndef TREEVIEWWINDOW_H
#define TREEVIEWWINDOW_H

#pragma once
#include "gui.hpp"

    struct AnyNode {    

        const char * name;    
        AnyNode(const char * name = "null") { this->name = name; }

        // Image icon;  

        virtual void drawNode() {                    
        
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TreeNode(name); 
        
        }

        void draw() { drawNode(); }

    };

    template <typename T>
    struct Node : AnyNode {

        static inline std::vector<T*> pool; 
	
    	Node(const char * label) : AnyNode(label) { pool.push_back((T*)this); }  
    
    };

    struct GroupNode : public AnyNode {

        GroupNode(const char * name = "Group") : AnyNode(name) {};

        std::vector<AnyNode*> tree;
    
        template <typename T>
        T* addNode(T* node) { tree.push_back(node); return (T*)tree.back(); }

        virtual void drawTree() { for (AnyNode* child : tree) child->draw(); }

        void drawNode() override {   
            
            ImGui::SetNextItemOpen(true, ImGuiCond_Once); // always open

            // if (ImGui::IsItemClicked()) current = this;

            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            bool tn = ImGui::TreeNode(name);
            
                    ImGui::TableNextColumn();
                    ImGui::Checkbox("", &tn);
                    ImGui::SameLine();
                    ImGui::SmallButton("+");

            if (tn) {

                drawTree();
                ImGui::TreePop();

            }

        }

    };

    template <typename T>
    struct PoolNode : public GroupNode {

        PoolNode(const char * label = "PoolNode" ) : GroupNode(label) {};

	    void drawTree() override { for (auto* child : T::pool) child->drawNode(); }

    };

    
    struct MasterNode : GroupNode {
    
    	MasterNode() : GroupNode("TREE_MASTER_NODE") { }  

        template <typename T>
        AnyNode* addPool(const char * label) { return addNode(new PoolNode<T>(label)); }

    };
    

struct TreeviewWindow : GUIRenderer, MasterNode {

    TreeviewWindow() {}

    void draw() { 

        ImGui::Begin("Tree", NULL, ImGuiWindowFlags_NoCollapse);
       if (ImGui::BeginTable("3ways", 2, ImGuiTableFlags_RowBg))
        {   ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("On/Off");
            ImGui::TableHeadersRow();

            GroupNode::drawTree();

            ImGui::EndTable();
        }

        ImGui::End();

    }

    // ~TreeviewWindow();

};

#endif