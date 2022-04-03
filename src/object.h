#ifndef OBJECT_H
#define OBJECT_H

#pragma once

#include <vector>
#include "src/gui.hpp"
// #include "src/attribute.hpp"


struct AnyObject { };
struct Object : AnyObject {

    enum Type {

        NONE,
        GROUPE,
        ATTRIBUTE,
        DRAWCALL

    } type;

    static inline std::vector<AnyObject> pool; // only Object owner

    const char * label = "null";    

    float color[3] = {1,1,1};

    // Image icon;  

    int id;

    char depth;

    Object() { id = pool.size(); pool.push_back(this); }

    // template <typename T>
    // T* add(T* node) { auto  return (T*)tree.back(); }

    virtual bool guiNode() { return ImGui::TreeNode(label); }

    virtual bool guiEditor() {}

    virtual void onUpdate() {}

};

struct Group : public Object {

    bool guiNode() override {   

        bool node = Object::guiNode();
        
        if (node) {

            // drawTree();

            ImGui::TreePop();

        }

        return node;

    }

};

#endif