#include "drawcall.hpp"
#include "shaderfx.hpp"
#include "model.hpp"
#include "gui.hpp"

DrawCall::DrawCall() : Node("drawcall") { }

void DrawCall::run() {
 
    vbo.draw();

    if (shader.loaded) shader.use();

}

void DrawCall::update() {

    // shader builder 

    // list fx

    std::unordered_set<ShaderFX*> shaders;

    for (auto model : childrens) { for (auto c : model->childrens) {
        shaders.insert(((ShaderFXPtr*)c)->ptr);
        } }


    // FRAGMENT

    std::string frag_shader;
    frag_shader = "#version 430 core\n\nout vec4 color;\n\n";

    // if buffer filled then 
    frag_shader += "layout (binding = 0, std140) uniform dynamic_ubo { float x[4]; };\n\n";

    for (auto shader : shaders) {

        frag_shader += ((ShaderFX*)shader)->code +"\n\n";

    }
    frag_shader += "void main() {\n\n";

    for (auto model : childrens) {

        frag_shader += "\t// " +((Model*)model)->name+"\n";

        frag_shader += "\tcolor = vec4(1);\n";

        for (auto c : model->childrens) { 

            auto shader = ((ShaderFXPtr*)c)->ptr;

            frag_shader += "\tcolor = ";
            frag_shader += shader->name + "(";
            frag_shader += "color";
            for (int i = 1; i < shader->args.size(); i++) frag_shader += ", x["+std::to_string(i-1)+"]";
            frag_shader += ")";
            frag_shader += ";\n";

         }

        frag_shader += "\n\n";

    }

    frag_shader += "}";


    /// VERTEX

    std::string vert_shader;
    vert_shader = "#version 430 core\n\n";
    vert_shader += "layout (location = 0) in vec2 POSITION;\n\n";

    vert_shader += "void main() {\n\n";

    vert_shader += "\tgl_Position = vec4(POSITION.x,POSITION.y,0,1);\n\n";

    vert_shader += "}";

    shader.create(frag_shader,vert_shader);

}

Node* DrawCall::add(Node *node) {

    if (node->is_a<Model>()) {
        
        Node::add(new ModelPtr(node));
        
        return node;

    }

    return nullptr;

}        

void DrawCall::editor() { 
    
    ImGui::InputTextMultiline("frag shader", &shader.frag.src[0], shader.frag.src.length(), ImVec2(300,300));
    ImGui::InputTextMultiline("vert shader", &shader.vert.src[0], shader.vert.src.length(), ImVec2(300,300));

    //imgui textbox

    
}