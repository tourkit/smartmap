#include "drawcall.hpp"
#include "shaderfx.hpp"
#include "model.hpp"
#include "gui.hpp"

DrawCall::DrawCall() : Node("drawcall") {


}

void DrawCall::run() {

    
    vbo.draw();

}

void DrawCall::update() {

    // shader builder 

    // list fx

    std::unordered_set<ShaderFX*> shaders;

    for (auto model : childrens) { for (auto c : model->childrens) shaders.insert(((Ptr<ShaderFX>*)c)->ptr); }

    frag_shader.resize(0);

    for (auto shader : shaders) {

        frag_shader += ((ShaderFX*)shader)->code +"\n";

    }

}

Node* DrawCall::add(Node *node) {

    struct ShaderFXPtr : Ptr<ShaderFX> {

        ShaderFXPtr(void* ptr) : Ptr<ShaderFX>(ptr) { }

        Node* add(Node *node) { return nullptr; }    

    };

    struct ModelPtr : Ptr<Model> {

        ModelPtr(void* ptr) : Ptr<Model>(ptr) { }
            
        Node* add(Node *node) {

            if (node->is_a<ShaderFX>()) {

                Node::add(new Ptr<ShaderFX>(node));

                parent()->update();

                return node;
                
            }

            return Node::add(node);

            return nullptr;

        }    

    };

    if (node->is_a<Model>()) return Node::add(new ModelPtr(node));

    return nullptr;

}        

void DrawCall::editor() { 
    
    ImGui::InputTextMultiline("frag shader", &frag_shader[0], frag_shader.length(), ImVec2(300,300));
    ImGui::InputTextMultiline("vert shader", &vert_shader[0], vert_shader.length(), ImVec2(300,300));

    //imgui textbox

    
}