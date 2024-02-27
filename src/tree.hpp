#pragma once
#include "gui.hpp"
#include "node.hpp"
#include "engine.hpp"

struct TreeWidget : GUI::Window {

    Node* selected;

    TreeWidget(Node* selected = nullptr);

    void draw() override;

    void drawNode(Node* node);   
    void drawChildrens(Node* node);   

    bool TreeViewNode(Node* node);

private:

bool is_deleting = false;

Node* is_renaming = nullptr;

};