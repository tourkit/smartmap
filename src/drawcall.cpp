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
    frag_shader += "out vec4 color;\n\n";

    // add fxs
    std::unordered_set<ShaderFX*> fxs;
    for (auto &m : vbo.models) for (auto fx : m.fxs) fxs.insert(fx);
    for (auto fx : fxs) frag_shader += fx->file->data +"\n\n";
    
    // main loop
    frag_shader += "\nvoid main() {\n\n\tcolor = vec4(0);\n\n";

    int model_id = 0;
    for (auto &model : vbo.models) {

        auto varname = model.file->name+""+std::to_string(model_id);

        frag_shader += "\t// " +varname+"\n";

        for (int instance = 0; instance < model.obj->reserved; instance++) {

            auto varinst = varname+"inst"+std::to_string(instance);

            frag_shader += "\tvec4 "+varinst+" = vec4(1,1,1,1);\n\n";
            
            if (model.obj->reserved) for (auto fx : model.fxs) { 

                if (!strcmp(fx->args[0].c_str(),"uv")) { continue; }

                PLOGD<<model.fxs[0]->args[0];

                frag_shader += "\t"+varinst+" = "+fx->file->name+"("+varinst;
                
                for (auto &m: Component::id(fx->file->name.c_str())->members) {
            
                    frag_shader += ", "+varname+std::to_string(instance)+"."+fx->file->name+"."+m.name+"";

                }
            
                frag_shader += ");\n";
            }

            frag_shader += "\n\tcolor += "+varinst+";\n\n";

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

