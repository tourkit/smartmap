#pragma once

#include "RectangleBinPack/MaxRectsBinPack.h"
#include "pch.hpp"
#include "texture.hpp"
#include "ubo.hpp"
#include "node.hpp"
#include "image.hpp"
#include "directory.hpp"

struct UBO;
 
struct Atlas : Node {

    rbp::MaxRectsBinPack binpack;

    std::vector<std::array<float,4>> normalized_list;

    Texture *texture;
         
    UBO *ubo; 

    std::string path;

    Atlas(int width = 4096, int height = 4096 , std::string path = "");

    void clear();

    void fromDir(std::string path);

    void link(ShaderProgram* shader);

    Node* add(Node *node) {

        if (node->is_a<Image>()) return Node::add(node);

        return nullptr;

    }   

    void editor() override;
};