#include "drawcall.hpp"
#include "effector.hpp"
#include "file.hpp"
#include "log.hpp"
#include "ubo.hpp"
#include "engine.hpp"
#include "atlas.hpp"
#include "texture.hpp"
#include "model.hpp"

#include <unordered_set>
#include <set>


Layer::Layer(uint16_t width, uint16_t height) : fb(width, height) {}

Layer::Layer() : fb(engine.window.width,engine.window.height) {}

void Layer::run() {

    fb.bind();

    DrawCall::run();

}



DrawCall::DrawCall() {

    // tofix 
    // engine.dynamic_ubo.subscribers.push_back(&shader);
    // engine.static_ubo.subscribers.push_back(&shader);

    // engine.atlas->link(&shader);

}

void DrawCall::run() {
 
     shader.sendUniform("medias", 1);
     engine.atlas->texture->bind();
    vbo.draw();

    shader.use(); 

}

static std::string struct_taber = "";//\t";
static std::string struct_spacer = " ";//\n\n";
static std::string comment_line  = "///////////////////////////////////////////\n\n";

std::string DrawCall::layout(UBO* ubo) {

    std::string comps_str;

    std::string obj_str;
    
    std::string content_str;

    std::set<Component*> components;

    // Object Struct

    bool is_reserved = false;

    // tofix


    // for (auto &obj : ubo->objects) {
        
    //     if (!obj.s->comps.size()) continue;

    //     obj_str += "struct "+camel(obj.s->name)+" {"+struct_spacer; 

    //     int comp_id = 0;

    //     int offset = 0;

    //     for (auto c: obj.s->comps) {

    //         auto size = c->size;

    //         if (c->members.size()<2) {

    //             obj_str += struct_taber+c->members[0].type_name()+" "+lower(c->name)+"; "+struct_spacer;

    //         } else {
                
    //             obj_str += struct_taber+camel(c->name)+" "+lower(c->name)+";"+struct_spacer;
            
    //             if (obj.s->is_striding) size = nextFactor(size,16);
            
    //         }
            
    //         components.insert(c);

    //         offset += size;
            
    //     }

    //     for (int i = 0; i< (nextFactor(offset,16)-offset)/sizeof(float); i++) obj_str += "float stride"+std::to_string(i)+";"+struct_spacer;

    //     obj_str += "};\n\n";

    //     content_str += camel(obj.s->name) + " " + lower(obj.s->name) + "[" + std::to_string(obj.reserved) + "];";

    //     if (obj.reserved) is_reserved = true;
        
    // }

    // // component Struct

    // for (auto c: components) {

    //     if (c->members.size()<2) continue;

    //     std::string comp_str = "struct "+camel(c->name)+" {"+struct_spacer;

    //     for (auto &m : c->members) comp_str += struct_taber+std::string(m.type_name())+" "+m.name+" ; "+struct_spacer;

    //     for (int i = 0; i< (nextFactor(c->size,16)-c->size)/sizeof(float); i++) comp_str += "float stride"+std::to_string(i)+";"+struct_spacer;

    //     comp_str += "};\n\n";

    //     comps_str += comp_str;
    // }

    std::string layout_str;
    
    // if (is_reserved && obj_str.length()){
    
    //     layout_str += comps_str + comment_line + obj_str + comment_line;

    //     layout_str += "layout (binding = "+std::to_string(ubo->binding)+", std140) uniform "+ubo->name+" { "+content_str+" };\n\n";
    
    // }
    
    return layout_str;


}

void DrawCall::update() {

    // COMMON

    std::string header_commom = "#version 430 core\n\n"+comment_line;

    header_commom += layout(&engine.dynamic_ubo);
    header_commom += layout(&engine.static_ubo);

    // FRAGMENT
    
    std::string frag_shader = header_commom;

    frag_shader += comment_line;

    frag_shader += "uniform sampler2D texture0;\n\n"; // foreach declared Texture::units maybe ? 
    frag_shader += "uniform sampler2D medias;\n\n";

    frag_shader += "in vec2 UV;\n\n";
    frag_shader += "out vec4 COLOR;\n\n";
    frag_shader += "vec4 color;\n\n";
    frag_shader += "vec2 uv;\n\n";

    frag_shader += comment_line;

    std::set<Effector*> effectors;

    // tofix
    
    // for (auto &m : vbo.models) for (auto effector : m->effectors) effectors.insert(effector);
    // for (auto effector : effectors) frag_shader += effector->source() +"\n";
    
    // main loop
    frag_shader += "\n"+comment_line;
    frag_shader += "void main() {\n\n";
    frag_shader += "\tCOLOR = vec4(0);\n\n";

    int model_id = 0;

    // tofix
    // for (auto &model : vbo.models) {

    //     for (int instance = 0; instance < model->obj->reserved; instance++) {

    //         auto name = model->file->name+std::to_string(model_id)+"["+std::to_string(instance)+"]";

    //         frag_shader += "\t// "+name+"\n"; // would love this to be a node name instead
    //         frag_shader += "\tuv = UV;\n";
    //         frag_shader += "\tcolor = vec4(1);\n";
            
    //         for (auto effector : model->effectors) { 
                
    //             std::string arg_str;

    //             auto comp = Component::id(effector->file->name.c_str()); 

    //             if (comp->members.size()<2) arg_str += name+"."+effector->file->name;

    //             else for (auto &m: comp->members) {
                    
    //                 arg_str += name+"."+effector->file->name+"."+m.name;

    //                 if (&m != &comp->members.back()) arg_str += ", ";
                    
    //             }

    //             frag_shader += "\t"+effector->file->name+"("+arg_str+");\n";
    //         }

    //         frag_shader += "\tCOLOR += color;\n\n";

    //     }


    //     frag_shader += "\n\n";

    //     model_id++;
    // }
    
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

