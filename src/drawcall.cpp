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

    std::string str = "";
    std::string in_str = "";

    int i = 0;
    bool at_least_one = false;
    for (auto m:vbo.models) { 
        auto obj = m.obj;
        if (!obj->reserved || !obj->s->comps.size()) continue;
        std::string name_lower = "";
        for(int i =  0; i < obj->s->name.length(); i++) name_lower += std::tolower(obj->s->name[i]); 
        std::string obj_str = name_lower;
        obj_str[0] = std::toupper(obj_str[0]);
    
        std::string struct_str = "struct "+obj_str+"{\n\n";

        int comp_id = 0;
        for (auto c: obj->s->comps) struct_str += "\tvec4 "+c->name+";\n";

        struct_str += "};\n\n";

        str += struct_str;

        if(obj->reserved)  {

            int instance = 0;

            in_str += obj_str+ " "+name_lower+std::to_string(instance)+"";

            for (int instance = 1; instance < obj->reserved; instance++) in_str+=", "+name_lower+std::to_string(instance);

            in_str+=";";

            at_least_one = true;

        }

    }

    if (at_least_one) str += "layout (binding = "+std::to_string(ubo->binding)+", std140) uniform "+ubo->name+" { "+in_str+" };\n";

    return str;

}

void DrawCall::update() {

    // FRAGMENT SHADER BUILDER

    std::string layout_str = "";//layout (binding = 0, std140) uniform dynamic_ubo { float x,y,z,w; };\n"; 
    layout_str += ""+Shaderlayout(engine.dynamic_ubo)+"\n";
    
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

        int instance = 0;
            
            for (int instance = 0; instance < model.obj->reserved; instance++) {
        auto varname = model.file->name+""+std::to_string(model_id)+"inst"+std::to_string(instance);

        frag_shader += "\tvec4 "+varname+" = vec4(1,1,1,1);\n\n";

        
        int comp_id = 0;
        if (model.obj->reserved) for (auto fx : model.fxs) { 

               frag_shader += "\t"+varname+" = "+fx->file->name+"("+varname;
               
               for (auto &m: Component::id(fx->file->name.c_str())->members) {
        

                    frag_shader += ", "+model.file->name+std::to_string(model_id)+std::to_string(instance)+"."+fx->file->name+"."+m.name+"";

                    
                }
               
                frag_shader += ");\n";
         }

        frag_shader += "\n\tcolor += "+varname+";\n\n";

        

        }

        frag_shader += "\n\n";

        model_id++;

    }

    // frag_shader += "\tcolor = vec4(v.x,v.y,v.z,1);\n\n";
    
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

