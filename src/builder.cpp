#include "builder.hpp"


#include "ubo.hpp"
#include "vbo.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "effector.hpp"
#include "instance.hpp"

#include "utils.hpp"
#include <string>

void Builder::post() {

    for (auto x : effectors_fragment) x->post(this);

    for (auto x : effectors_vertex) x->post(this);

    int id = 0;
    

    for (auto s : samplers) {
        
        if (s.first > id) {
            // for ( int i = id; i < s.first; i++) shader->sendUniform("randi"+std::to_string(i), i);
            id = s.first;
        }
        s.second->bind(s.first);
        shader->sendUniform(s.second->sampler_name, s.first);
        
        id++;
    }

}
void Builder::build() {

    header_common = version;
    header_common += layout();

    samplers.clear();
    
    if (!shader) return;

    for (auto x : ubos) x->bind(shader);

}

std::string Builder::frag() {

    std::string samplers_str; 

    int id = 0;
    
    for (auto x : samplers) {

        if (x.first > id) {
            // for ( int i = id; i < x.first; i++) samplers_str += "uniform sampler2D randi"+std::to_string(i)+";\n"; 
            id = x.first;
        }

        samplers_str += "uniform sampler2D "+x.second->sampler_name+";\n"; 
        id++;

    }
    
    if (samplers.size()) samplers_str += "\n";
    

    std::string effectors_str; for (auto x : effectors_fragment)  effectors_str += x->source()+"\n\n";

    std::string ins_str;

    for (int i = 1; i < vbo->vertice.members.size(); i++) {

        auto m = vbo->vertice.members[i];

        ins_str += "in "+std::string(m->type().id == typeid(int)?"flat ":"")+m->type_name()+" "+m->name()+";\n";

    } if (ins_str.length()) ins_str += "\n";

    return

        header_common +

        "out vec4 COLOR;\n\n" +

        ins_str +

        samplers_str + (samplers_str.length()?comment_line:"") +

        header_fragment + (header_fragment.length()?comment_line:"") +

        effectors_str + (effectors_str.length()?comment_line:"") +

        "void main() {\n\n" +

        (body_fragment.length()?body_fragment:"\tCOLOR = vec4(0,1,0,1);\n\n") +

        "}\n\n" + comment_line;

}

std::string Builder::vert() {

    vbo_layouts.clear();

    if (vbo) {

        int count = 0;

        for (auto x : vbo->vertice.members)

            vbo_layouts.push_back({count++,x->type_name(), (x->name()+(count>1?"_":""))});

    }

    if (!vbo_layouts.size()) vbo_layouts.push_back({0,"vec2","POSITION"});

    std::string layouts_str; for (auto l : vbo_layouts) layouts_str += "layout (location = "+std::to_string(l.loc)+") in "+l.type+" "+l.name+";\n"; if (samplers.size()) layouts_str += "\n";

    std::string effectors_str; for (auto x : effectors_vertex)  effectors_str += x->source()+"\n\n";

    return

        header_common + comment_line +

        layouts_str + (layouts_str.length()?comment_line:"") +

        header_vertex + (header_vertex.length()?comment_line:"") +

        effectors_str + (effectors_str.length()?comment_line:"") +

        "void main() {\n\n" +

        (body_vertex.length()?body_vertex:"\tvec2 POS = POSITION;\n\n\tgl_Position = vec4(POS, 0, 1);\n\n") +

        "}\n\n" + comment_line;

}


bool Builder::add(UBO* ubo) { return true; }

std::string Builder::layout() {

    std::vector<Member*> structs;

    for (auto ubo : ubos)    {

        Instance(*ubo).each([&](Instance& inst) {

            auto m = inst.stl.back().m;

            if (!m->isData())
                ADD_UNIQUE<Member*>(structs,m);

        });

        ADD_UNIQUE<Member*>(structs,ubo);

    }

    std::string out;

    std::map<Member*,std::string> unique_name_list;

    for (auto m : structs) {

        auto name = camel(m->name());

        while (true) {

            for (auto &x : unique_name_list) if (x.second == name) {
                
                if (name[name.length()-1] == '_') name += "0";
                else name += "_";

                continue;

            }

            break;

        }

        unique_name_list[m] = name;

    }

    for (auto x : structs) { auto def = print_struct(x,unique_name_list); if (def.length()) def+=";\n\n"; out += def; }

    for (auto ubo : ubos) {

        out += "layout (binding = " + std::to_string(ubo->binding) + ", std140) uniform " + ubo->name() + "_ ";

        out += " { " + camel(ubo->name()) + " " + lower(ubo->name());

        if (ubo->quantity()>1) out += "["+std::to_string(ubo->quantity())+"]";

        out += + "; };\n\n";

    }

    return out;

}

std::string Builder::print_struct(Member* member, std::map<Member*,std::string> &unique_name_list) {

    if (!member->size()) return "";

    std::string out;
    std::string nl = "";
    std::string tb = "";
    // if (member->members.size() == 1) nl = "";

    std::string content;

    for (auto x : member->members) {

        if (!x->size()) continue;

        auto name = unique_name_list.find(x)!=unique_name_list.end()?unique_name_list[x]:x->type_name();

        content+=tb+""+name+" "+lower(x->name());

        if (!x->isData() && x->quantity()>1) content += "["+std::to_string(x->quantity())+"]";

        content += "; "+nl;

    }

    if (!content.length()) return "";

    out+="struct "+unique_name_list[member]+" { "+nl+nl+content;

    if (member->stride()) for (int i = 0; i < member->stride()/sizeof(float); i++) {

        out += tb;
        out += (member->members.back()->type().id == typeid(int) ? "int" : "float");
        out += " stride";
        out += std::to_string(i) + "; "+nl ;

    }

    out+=nl+"}";
    return out;

}
