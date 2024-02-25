#include "drawcall.hpp"
#include "effector.hpp"
#include "file.hpp"
#include "log.hpp"
#include "ubo.hpp"
#include "engine.hpp"

#include <unordered_set>
#include <set>


Layer::Layer(uint16_t width, uint16_t height) : fb(width, height) {}

Layer::Layer() : fb(engine.window.width,engine.window.height) {}

void Layer::run() {

    fb.bind();

    DrawCall::run();

}


static std::string camel(std::string str) { str[0] = std::toupper(str[0]); return str; }

DrawCall::DrawCall() {

    engine.dynamic_ubo->subscribers.push_back(&this->shader);

}

void DrawCall::run() {
 
    vbo.draw();

    shader.use(); 

}

static std::string struct_taber = "";//"\t";
static std::string struct_spacer = " ";//"\n\n";

std::string DrawCall::layout(UBO* ubo) {

    std::string out = "";
    
    std::string attr_str = "";

    bool is_reserved = false;

    std::set<Component*> components;

    for (auto m:vbo.models) { 

        auto obj = m->obj; if (!obj->reserved || !obj->s->comps.size()) continue;

        std::string name = ""; for(int i =  0; i < obj->s->name.length(); i++) name += std::tolower(obj->s->name[i]); 

        std::string Name = camel(name);
    

        std::string struct_str = "struct "+Name+"{"+struct_spacer;

        int comp_id = 0;

        for (auto c: obj->s->comps) {
            
            struct_str += struct_taber+camel(c->name)+" "+c->name+";"+struct_spacer;
            
            components.insert(c);
            
        }

        struct_str += "};\n\n";

        out += struct_str;

        if(obj->reserved)  {

            attr_str += Name+ " "+name+"["+std::to_string(obj->reserved)+"]";

            attr_str+=";";

            is_reserved = true;

        }

    }

    // component Struct

    std::string comps_str = "";

    for (auto c: components) {

        std::string comp_str = "struct "+camel(c->name)+" {"+struct_spacer;

        for (auto &m : c->members) {
            
            std::string type = "float";

            if (m.type == Member::Type::VEC2) type = "vec2";

            comp_str += struct_taber+type+" "+m.name+";"+struct_spacer;
            
        }

        comp_str += "};\n\n";

        comps_str += comp_str;
    }

    if (is_reserved) { 
        
        out = comps_str+out+"layout (binding = "+std::to_string(ubo->binding)+", std140) uniform "+ubo->name+" { "+attr_str+" };\n\n";
    
    }

    return out;

}

void DrawCall::update() {

    // COMMON

    std::string header_commom = "#version 430 core\n\n";

    header_commom += layout(engine.dynamic_ubo);

    // FRAGMENT
    
    std::string frag_shader = header_commom;
    frag_shader += "in vec2 UV;\n\n";
    frag_shader += "out vec4 COLOR;\n\n";
    frag_shader += "vec4 color;\n\n";
    frag_shader += "vec2 uv;\n\n";

    std::set<Effector*> effectors;
    for (auto &m : vbo.models) for (auto effector : m->effectors) effectors.insert(effector);
    for (auto effector : effectors) frag_shader += effector->source() +"\n";
    
    // main loop
    frag_shader += "\nvoid main() {\n\n";
    frag_shader += "\tCOLOR = vec4(0);\n\n";

    int model_id = 0;
    for (auto &model : vbo.models) {

        for (int instance = 0; instance < model->obj->reserved; instance++) {

            auto name = model->file->name+std::to_string(model_id)+"["+std::to_string(instance)+"]";

            frag_shader += "\t// "+name+"\n";
            frag_shader += "\tuv = UV;\n";
            frag_shader += "\tcolor = vec4(1);\n";
            
            for (auto effector : model->effectors) { 
                
                std::string arg_str;

                auto comp = Component::id(effector->file->name.c_str());

                for (auto &m: comp->members) {
                    
                    arg_str += name+"."+effector->file->name+"."+m.name;

                    if (&m != &comp->members.back()) arg_str += ", ";
                    
                }

                frag_shader += "\t"+effector->file->name+"("+arg_str+");\n";
            }

            frag_shader += "\tif (uv.x+uv.y>0) COLOR += color;\n\n";

        }

        frag_shader += "\n\n";

        model_id++;

    }
    
    frag_shader += "} ";


    // VERTEX

    std::string vert_shader = header_commom;

    vert_shader += "layout (location = 0) in vec2 POSITION;\n";
    vert_shader += "layout (location = 1) in vec2 TEXCOORD;\n";
    vert_shader += "layout (location = 3) in int OBJ;\n\n";

    vert_shader += "out vec2 UV;\n\n";

    vert_shader += "\nvoid main() {\n\n";

    vert_shader += "\tUV = TEXCOORD;\n";
    vert_shader += "\tUV.y = 1-UV.y;\n\n";

    vert_shader += "\tgl_Position = vec4(POSITION.x,POSITION.y,0,1);\n\n";

    vert_shader += "}";

    shader.create(frag_shader,vert_shader);

}

