#pragma once

#include "RectangleBinPack/MaxRectsBinPack.h"
#include "node.hpp"
#include "image.hpp"

struct UBO; struct ShaderProgram; struct Image; struct Texture;
 
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

    struct ImagePtr : Ptr<Image> {
        
        Atlas* atlas;

        ImagePtr(void* ptr, Atlas* atlas) : Ptr<Image>(ptr), atlas(atlas) {}
    
        void editor() override { atlas->editor(); }  

        // ~ImagePtr() { delete ptr; }

    };

    Node* add(Node *node) {

        if (node->is_a<Image>()) return Node::add(new ImagePtr(node, this));

        return nullptr;

    }   

    void editor() override;
};