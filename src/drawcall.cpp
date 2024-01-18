#include "drawcall.hpp"
#include "shaderfx.hpp"
#include "model.hpp"

DrawCall::DrawCall() {

    name = "drawcall";
}

Node* DrawCall::add(Node *node) {

    struct ShaderFXPtr : Ptr<ShaderFX> {

        ShaderFXPtr(void* ptr) : Ptr<ShaderFX>(ptr) { }

        Node* add(Node *node) { return nullptr; }    

    };

    struct ModelPtr : Ptr<Model> {

        ModelPtr(void* ptr) : Ptr<Model>(ptr) { }
            
        Node* add(Node *node) {

            if (node->is_a<ShaderFX>()) return Node::add(new Ptr<ShaderFX>(node));

            return nullptr;

        }    

    };

    if (node->is_a<Model>()) return Node::add(new ModelPtr(node));

    return nullptr;

}        