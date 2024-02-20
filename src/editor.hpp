#pragma once

#include "gui.hpp"

struct Node;

struct EditorWidget : GUI::Window {

    Node* selected = nullptr;

    bool locked = false;

    EditorWidget();

    void draw() override;

};

struct Editors  { static void init(); };