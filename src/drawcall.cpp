#include "drawcall.hpp"
#include "shaderfx.hpp"
#include "file.hpp"
#include "log.hpp"
#include "ubo.hpp"
#include "engine.hpp"

#include <unordered_set>


DrawCall::DrawCall() {

    engine.dynamic_ubo->subscribers.push_back(&this->shader);

}

void DrawCall::run() {
 
    vbo.draw();

    if (shader.loaded) shader.use();

}

std::string DrawCall::Shaderlayout(UBO* ubo) {

    std::string str = "layout (binding = ";
    str += std::to_string(ubo->binding);
    str += ", std140) uniform ";
    str += ubo->name;
    str += " { ";
    
    int i = 0; for (auto obj:ubo->buffer.objects) { 

        if(!obj.reserved) continue;
    
        std::string obj_str = obj.s->name;
        obj_str[0] = std::toupper(obj_str[0]);
        str += obj_str;
        str += " ";
        for(int i = 0; i < obj.s->name.length(); i++) str += std::tolower(obj.s->name[i]); 
        str += "[";
        str += std::to_string(obj.reserved);
        str += "]; ";

        }

    str += " };\n";

    return str;

}

void DrawCall::update() {

    // FRAGMENT SHADER BUILDER

    std::string layout_str = "layout (binding = 0, std140) uniform dynamic_ubo { float x[4]; };\n"; 
    layout_str += "// "+Shaderlayout(engine.dynamic_ubo)+"\n";
    
    std::string frag_shader;

    frag_shader = "#version 430 core\n\n";

    frag_shader += "out vec4 color;\n\n";

    frag_shader += layout_str;
    

    std::unordered_set<ShaderFX*> fxs;
    for (auto &m : vbo.models) for (auto fx : m.fxs) fxs.insert(fx);
    for (auto fx : fxs) frag_shader += fx->file->data +"\n\n";
    
    frag_shader += "\nvoid main() {\n\n";

    frag_shader += "\tcolor = vec4(0);\n\n";

    int model_id = 0;
    for (auto &model : vbo.models) {

        frag_shader += "\t// " +model.file->name+"\n";

        auto varname = model.file->name+"_"+std::to_string(model_id);

        frag_shader += "\tvec4 "+varname+" = vec4(1,1,1,1);\n\n";

        for (auto fx : model.fxs) { 

               frag_shader += "\t"+varname+" = "+fx->file->name+"("+varname;
               
               for (int i = 1; i < fx->args.size(); i++) { frag_shader += ",x["+std::to_string(i-1)+"]"; }
            //    frag_shader += ",x.x, x.y, x.z"; 
               
               frag_shader += ");\n";

         }

        frag_shader += "\n\tcolor += "+varname+";\n";

        frag_shader += "\n\n";

        model_id++;

    }
    
    frag_shader += "}";


    // /// VERTEX

    std::string vert_shader;
    vert_shader = "#version 430 core\n\n";

    vert_shader += "layout (location = 0) in vec2 POSITION;\n";
    vert_shader += "layout (location = 1) in vec2 TEXCOORD;\n";
    vert_shader += "layout (location = 3) in int OBJ;\n\n";

    vert_shader += layout_str;

    vert_shader += "\nvoid main() {\n\n";

    vert_shader += "\tgl_Position = vec4(POSITION.x,POSITION.y,0,1);\n\n";

    vert_shader += "}";

    shader.create(frag_shader,vert_shader);

}

