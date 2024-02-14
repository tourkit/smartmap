#pragma once

#include "gui.hpp"
#include "node.hpp"

struct EditorWidget : GUI::Window {

    UntypedNode* selected = nullptr;

    bool locked = false;

    EditorWidget();

    void draw() override;

};