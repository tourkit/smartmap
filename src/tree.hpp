#pragma once
#include "gui.hpp"
#include "node.hpp"

struct TreeWidget : GUI::Window {

    Node* selected = nullptr;

    bool locked = true;

    TreeWidget();

    void draw() override;

    void drawNode(Node* node);   
    void drawChildrens(Node* node);   

private:

bool is_deleting = false;

};