#include "ubo.hpp"

UBO::~UBO() { destroy(); }

UBO::UBO(std::string name, std::vector<Shader*> subscribers) : name(name), subscribers(subscribers) { 
    
    pool.push_back(this); 
    
} 

void UBO::addStruct(std::string name, std::vector<Component*> components = {}, size_t quantity) {

    definition.push_back(Struct(name, components, quantity));

    resize(); 

    widget.updateStructList(); 
    
}
void UBO::destroy() { data.resize(0); subscribers.resize(0);  if (id) glDeleteBuffers(1, &id); } // delete name; ?

void UBO::resize() {

    int size = 0;
    for (auto s:definition) for (auto &c:s.components) { size+= c->members.size()*s.quantity; }
    
    auto t_subscribers = subscribers; 
    
    destroy();

    data.resize(size);
    memset(&data[0],0,size*4);
    
    // can do better ^^
    if (binding > 100) std::cout << "MAX_UBO might soon be reached";

    glGenBuffers(1, &id);

    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferData(GL_UNIFORM_BUFFER, 4*data.size(), NULL, GL_DYNAMIC_COPY);

    std::cout << "RTFM /!\\ put good bindings in shader !! layout(std140, binding = " << binding << ") uniform " << name << " { size:" << data.size() << " };" << std::endl;

    subscribers = t_subscribers;
    for (auto shader:subscribers) link(shader->id); 

}

void UBO::link(GLuint shader) {

    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glUniformBlockBinding(shader, glGetUniformBlockIndex(shader, name.c_str()), binding);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, id);
}

void UBO::update(){ update(&data[0], 4*data.size()); }

void UBO::update(GLvoid* data, size_t size, GLuint offset){

    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data); 
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////            JSON                   //////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#include "include/vendor/rapidjson/document.h"
#include "include/vendor/rapidjson/stringbuffer.h"

void UBO::fromJSON(){ 

    // using namespace rapidjson;

    rapidjson::Document json;
    json.Parse(File(REPO_DIR+"show/ubo.json").data.data());

    if(json.HasParseError()) { std::cout << "ubo parse error" << std::endl; return; }

    if (!json.HasMember("ubo_list")) { std::cout << "Missing ubo_list" << std::endl; return; }

    for (auto &ubo : json["ubo_list"].GetArray()) { 

        if (!ubo.HasMember("name")) { std::cout << "Missing ubo name" << std::endl; return; }
        if (!ubo.HasMember("definition")) { std::cout << "Missing ubo definition" << std::endl; return; }

        std::cout << ubo["name"].GetString() << std::endl;

        UBO* target = nullptr;
        for (auto &existing_ubo:UBO::pool) { 
            if (!strcmp(existing_ubo->name.c_str(),ubo["name"].GetString())) {
                target = existing_ubo;
                break;
            }
        }
        if (!target) target = new UBO(ubo["name"].GetString());
        
        for (auto &def : ubo["definition"].GetArray()) { 

            if (!def.HasMember("name")) { std::cout << "Missing def name" << std::endl; return; }
            if (!def.HasMember("quantity")) { std::cout << "Missing def quantity" << std::endl; return; }
            if (!def.HasMember("components")) { std::cout << "Missing def definition" << std::endl; return; }

            std::vector<Component*> components;
            for (auto &comp : def["components"].GetArray()) { 

                if (!comp.HasMember("name")) { std::cout << "Missing comp name" << std::endl; return; }
                components.push_back(Component::id(comp["name"].GetString()));

            }

            target->addStruct(def["name"].GetString(), components, def["quantity"].GetInt());

        }

    }

    widget.updateUBOList();

}

#include "include/vendor/rapidjson/prettywriter.h"

void UBO::toJSON(){ 

    using namespace rapidjson;

    StringBuffer sb;
    PrettyWriter<StringBuffer> writer(sb);

    writer.StartObject();
    writer.String("ubo_list");
    writer.StartArray();

    for (auto ubo : UBO::pool) { 

        writer.StartObject();
        writer.String("name");
        writer.String(ubo->name.c_str());
        writer.String("definition");
        writer.StartArray();

        for (auto def:ubo->definition) { 
            
            writer.StartObject();
            writer.String("name");
            writer.String(def.name.c_str());
            writer.String("components");
            writer.StartArray();

            for (auto comp:def.components) { 
                
                writer.StartObject();
                writer.String("name");
                writer.String(comp->name.c_str());

                writer.EndObject();

            }

            writer.EndArray();
            writer.String("quantity");
            writer.Int(def.quantity);
            writer.EndObject();

        }

        writer.EndArray();
        writer.EndObject();

    }

    writer.EndArray();
    writer.EndObject();

    File::write(REPO_DIR+"show/ubo.json", sb.GetString());

}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////               GUI                 //////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void UBO::Widget::draw() {

    ImGui::InputText("##NewUBO", &add_ubo[0], add_ubo.size());
    ImGui::SameLine();
    if (ImGui::Button("Add UBO") && strlen(&add_ubo[0])) { 
        
        UBO *tmp = new UBO(add_ubo.c_str());

        ubo_current = UBO::pool.size()-1;  
        
        memset(&add_ubo[0],0,add_ubo.size());

        UBO::toJSON();
    
    }
    ImGui::Spacing();
    if (ImGui::Combo("UBO", &ubo_current, ubo_list.buffer)) updateStructList();

    ImGui::NewLine();

    if (subs_list.buffer) {
        
        ImGui::Combo("##subscriberzzz", &add_sub, subs_list.buffer);
        ImGui::SameLine();
        if (ImGui::Button("Add sub")) { 
        
        
        }

        ImGui::NewLine();
        
    }

    if (!UBO::pool.size()) return;

    auto *ubo = UBO::pool[ubo_current];

    ImGui::InputText("##NewStruct", &add_struct[0], add_struct.size());
    ImGui::SameLine();
    if (ImGui::Button("Add Struct") && strlen(&add_struct[0])) { 
        
        ubo->addStruct(add_struct); 

        struct_current = ubo->definition.size()-1;  
        
        memset(&add_struct[0],0,add_struct.size());

        UBO::toJSON();
    
    }
    ImGui::Spacing();

    if (!ubo->definition.size()) return;

    ImGui::Combo("struct", &struct_current, struct_list.buffer);

    auto &def = ubo->definition[struct_current];
    
    if (ImGui::InputInt("quantity##structquantity", &def.quantity)) {
        
        if (!def.quantity) {
            
            ubo->definition.erase(ubo->definition.begin()+struct_current);
            widget.updateUBOList();
            ubo->resize();
            return;
        }

        ubo->resize();

        UBO::toJSON();
        
    }

    int uniform_offset = 0;
    if (ImGui::SliderInt("current##uibocurrent", &elem_current,0,def.quantity-1)) UBO::toJSON();
    
    ImGui::NewLine();
    
    ImGui::Combo("##AddComponent", &add_comp, comp_list.buffer);
    ImGui::SameLine();
    if (ImGui::Button("Add Component")) {

        def.components.push_back(Component::pool[add_comp]); 
        ubo->resize(); 
        UBO::toJSON();

    }

    ImGui::Spacing();
    int members_size = 0;
    for (auto &c:def.components) { members_size+= c->members.size(); }

    for (auto c:def.components) {
        
        if (ImGui::CollapsingHeader(c->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
        
            for (auto m:c->members) {

            if (ImGui::SliderFloat((m.name+"##"+c->name+m.name+uid).c_str(), &ubo->data[uniform_offset+++(elem_current*members_size)], m.range_from, m.range_to)) { 
                
                ubo->update(); 
            }
            
            }
            
        } else { uniform_offset += c->members.size(); }
            
    }

    ImGui::NewLine();
    
    ImGui::NewLine();


}
    