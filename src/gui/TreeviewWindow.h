#ifndef TREEVIEWWINDOW_H
#define TREEVIEWWINDOW_H

#pragma once
#include "gui.hpp"

    struct Node {    

        const char * name;    
        Node(const char * name = "null") { this->name = name; }

        // Image icon;  

        virtual void drawNode() { if (ImGui::TreeNode(name)) ImGui::TreePop(); }

        void draw() { drawNode(); }

    };

    struct GroupNode : public Node {

        GroupNode(const char * name = "Group") : Node(name) {};

         std::vector<Node*> tree;
    
        template <typename T>
        T* addNode(T* node) { tree.push_back(node); return (T*)tree.back(); }

        virtual void drawTree() { for (Node* child : tree) child->draw(); }

        void drawNode() override {   
            
            ImGui::SetNextItemOpen(true, ImGuiCond_Once); // always open

            bool tn = ImGui::TreeNode(name);

            // if (ImGui::IsItemClicked()) current = this;
            if (tn) {
                drawTree();
                ImGui::TreePop();
            }
        }

    };



struct TreeviewWindow : GUIRenderer, GroupNode {

    TreeviewWindow() {}

    void draw() { 

        ImGui::Begin("Tree", NULL, ImGuiWindowFlags_NoCollapse);

        GroupNode::drawTree();

        ImGui::End();

    }

    // ~TreeviewWindow();

};

#endif