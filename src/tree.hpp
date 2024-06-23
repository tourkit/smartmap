#pragma once
#include "gui.hpp"
#include "node.hpp"
#include "engine.hpp"

#include <vector>

struct TreeWidget : GUI::Window {

    Node* selected;

    TreeWidget(Node* selected = nullptr);

    void draw() override;

    void drawNode(Node* node);
    void drawChildrens(Node* node);

    bool TreeViewNode(Node* node);


    std::string filter_str = "filter";

    char search_str[100];

    bool filtering = false;

private:

bool is_deleting = false;

Node* is_renaming = nullptr;
std::array<char,512> renaming_name;


};
