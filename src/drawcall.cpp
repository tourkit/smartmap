#include "drawcall.hpp"
#include "shaderfx.hpp"
#include "file.hpp"
#include "log.hpp"

#include <unordered_set>


void DrawCall::run() {
 
    vbo.draw();

    if (shader.loaded) shader.use();

}

void DrawCall::update() {

    // // shader builder 

    // // list fx

    // for (auto model : childrens) { for (auto c : model->childrens) {
    //     shaders.insert(((ShaderFXPtr*)c)->ptr);i
    //     } }


    // FRAGMENT

    std::string frag_shader;
    frag_shader = "#version 430 core\n\n";
    frag_shader += "out vec4 color;\n\n";

    // // if buffer filled then 
    // frag_shader += "layout (binding = 0, std140) uniform dynamic_ubo { float x[4]; };\n\n";

    std::unordered_set<ShaderFX*> fxs;
    // for (auto &m : vbo.models) for (auto fx : m.fxs) fxs.insert(fx);
    // for (auto fx : fxs) frag_shader += fx->file->data +"\n\n";

    PLOGD << fxs.size();
    
    frag_shader += "void main() {\n\n";

    // for (auto model : childrens) {

    //     frag_shader += "\t// " +((Model*)model)->name+"\n";

    //     frag_shader += "\tcolor = vec4(1);\n";

    //     for (auto c : model->childrens) { 

    //         auto shader = ((ShaderFXPtr*)c)->ptr;

    //         frag_shader += "\tcolor = ";
    //         frag_shader += shader->name + "(";
    //         frag_shader += "color";
    //         for (int i = 1; i < shader->args.size(); i++) frag_shader += ", x["+std::to_string(i-1)+"]";
    //         frag_shader += ")";
    //         frag_shader += ";\n";

    //      }

        // frag_shader += "\n\n";

    // }

    frag_shader += "\tcolor = vec4(1);\n\n";
    
    frag_shader += "}";


    // /// VERTEX

    std::string vert_shader;
    vert_shader = "#version 430 core\n\n";

    vert_shader += "layout (location = 0) in vec2 POSITION;\n\n";
    vert_shader += "layout (location = 1) in vec2 TEXCOORD;\n\n";
    vert_shader += "layout (location = 3) in int OBJ;\n\n";

    vert_shader += "void main() {\n\n";

    vert_shader += "\tgl_Position = vec4(POSITION.x,POSITION.y,0,1);\n\n";

    vert_shader += "}";

    shader.create(frag_shader,vert_shader);

}

