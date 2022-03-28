#ifndef TREEVIEWWINDOW_H
#define TREEVIEWWINDOW_H

#pragma once
#include "gui.hpp"

    struct Node {    

        const char * name;    
        Node(const char * name = "null") { this->name = name; }

        // Image icon;  

        virtual void draw() { if (ImGui::TreeNode(name)) ImGui::TreePop(); }

    };

    struct GroupNode : public Node {

        GroupNode(const char * name = "Group") : Node(name) {};

         std::vector<Node*> tree;
    
        template <typename T>
        T* addNode(T* node) { tree.push_back(node); return (T*)tree.back(); }

        void drawTree() { for (Node* child : tree) child->draw(); }

        void draw() override {   
            
            ImGui::SetNextItemOpen(true, ImGuiCond_Once); // always open

            bool tn = ImGui::TreeNode(name);

            // if (ImGui::IsItemClicked()) current = this;
            if (tn) {
                drawTree();
                ImGui::TreePop();
            }
        }

    };


struct TreeviewWindow : GUI::Renderer, GroupNode {

    TreeviewWindow() {}

    void draw() { 

        ImGui::Begin("Tree", NULL, ImGuiWindowFlags_NoCollapse);

        GroupNode::drawTree();

        ImGui::End();

    }

    // ~TreeviewWindow();

};

#endif