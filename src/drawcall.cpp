#include "drawcall.hpp"
#include "effector.hpp"
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

    shader.use(); 

}

std::string DrawCall::Shaderlayout(UBO* ubo) {

    std::string out = "";
    
    std::string attr_str = "";

    bool is_reserved = false;

    for (auto m:vbo.models) { 

        auto obj = m.obj; if (!obj->reserved || !obj->s->comps.size()) continue;

        std::string name = ""; for(int i =  0; i < obj->s->name.length(); i++) name += std::tolower(obj->s->name[i]); 

        std::string Name = name; Name[0] = std::toupper(Name[0]);
    
        std::string struct_str = "struct "+Name+"{\n\n";

        int comp_id = 0;

        for (auto c: obj->s->comps) struct_str += "\tvec4 "+c->name+";\n";

        struct_str += "};\n\n";

        out += struct_str;

        if(obj->reserved)  {

            attr_str += Name+ " "+name+"0"; //inst0

            for (int instance = 1; instance < obj->reserved; instance++) attr_str+=", "+name+std::to_string(instance);

            attr_str+=";";

            is_reserved = true;

        }

    }

    if (is_reserved) out += "layout (binding = "+std::to_string(ubo->binding)+", std140) uniform "+ubo->name+" { "+attr_str+" };\n";

    return out;

}

void DrawCall::update() {

    // Common Header

    std::string header_commom = "#version 430 core\n\n";

    header_commom += ""+Shaderlayout(engine.dynamic_ubo)+"\n";
    
    // FRAGMENT
    
    std::string frag_shader = header_commom;
    frag_shader += "out vec4 out_color;\n\nvec4 color;\n\n";

    // add effectors
    std::unordered_set<Effector*> effectors;
    for (auto &m : vbo.models) for (auto effector : m.effectors) effectors.insert(effector);
    for (auto effector : effectors) frag_shader += effector->file->data +"\n\n";
    
    // main loop
    frag_shader += "\nvoid main() {\n\n\tout_color = vec4(0);\n\n\tvec2 uv = vec2(0);\n\n";

    int model_id = 0;
    for (auto &model : vbo.models) {

        auto varname = model.file->name+""+std::to_string(model_id);

        for (int instance = 0; instance < model.obj->reserved; instance++) {

            auto varinst = varname+"inst"+std::to_string(instance);

            frag_shader += "\t// " +varname+"\n\n"+"\tcolor = vec4(1);\n\n";
            
            for (auto effector : model.effectors) { 
                
                std::string arg_str = "";
                for (auto &m: Component::id(effector->file->name.c_str())->members) {
                    
                    arg_str += varname+std::to_string(instance)+"."+effector->file->name+"."+m.name;
                    
                    if (&m != &Component::id(effector->file->name.c_str())->members.back()) arg_str += ", ";
                    
                }

                frag_shader += "\t"+effector->file->name+"("+arg_str+");\n\n";
            }

            frag_shader += "\tout_color += color;\n\n";

        }

        frag_shader += "\n\n";

        model_id++;

    }
    
    frag_shader += "}";

    // VERTEX

    std::string vert_shader = header_commom;

    vert_shader += "layout (location = 0) in vec2 POSITION;\n";
    vert_shader += "layout (location = 1) in vec2 TEXCOORD;\n";
    vert_shader += "layout (location = 3) in int OBJ;\n\n";

    vert_shader += "\nvoid main() {\n\n";

    vert_shader += "\tgl_Position = vec4(POSITION.x,POSITION.y,0,1);\n\n";

    vert_shader += "}";

    shader.create(frag_shader,vert_shader);

}

