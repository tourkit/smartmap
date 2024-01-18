#pragma once
#include "gui.hpp"
#include "node.hpp"

struct Tree : Node, GUI::Window {

    Tree();

    void draw() override;

    void drawNode(Node* node);   
    void drawChildrens(Node* node);   

};

struct ImRect
{
    ImVec2      Min;  
    ImVec2      Max; 
};