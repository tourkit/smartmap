#pragma once
#include "gui.hpp"
#include "node.hpp"

struct TreeWidget : GUI::Window {

    Node* selected = nullptr;
    std::vector<Node*> visible_list;

    TreeWidget(GUI* gui = nullptr);

    void draw() override;

    bool TreeViewNode(Node* node, int depth = 0);


    std::string filter_str = "filter";

    std::array<char,100> search_str;

    bool filtering = false;

private:

std::regex pattern; bool pattern_error = false;

bool is_deleting = false;

Node* is_renaming = nullptr;
std::array<char,512> renaming_name;


};
