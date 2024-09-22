#pragma once
#include <regex>
#include "gui.hpp"
#include "node.hpp"

struct TreeWidget : GUI::Window {

    Node* selected = nullptr;
    std::vector<Node*> visible_list;

    TreeWidget(GUI* gui = nullptr);

    void draw() override;

    static inline std::array<float,4> color_v = {1,1,1,1};

    bool TreeViewNode(Node* node, int depth=0, std::array<float,4>& color=color_v);

    std::array<char,100> search_str;

    bool filtering = false;

private:

std::regex pattern; bool pattern_error = false;

bool is_deleting = false;

};
