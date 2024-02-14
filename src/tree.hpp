#pragma once
#include "gui.hpp"
#include "node.hpp"
#include "engine.hpp"

struct TreeWidget : GUI::Window {

    UntypedNode* selected;

    TreeWidget(UntypedNode* selected = nullptr);

    void draw() override;

    void drawNode(UntypedNode* node);   
    void drawChildrens(UntypedNode* node);   

private:

bool is_deleting = false;

};