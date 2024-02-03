#include "ubo.hpp"

#include "shader.hpp"

UBO::~UBO() { destroy(); }

UBO::UBO(std::string name, std::vector<ShaderProgram*> subscribers) : Node(name), subscribers(subscribers) { 
    
    binding = binding_count++;

    // can do better ^^
    if (binding > 100) PLOGW << "MAX_UBO might soon be reached";
    
    update();

    buffer.callback = [this](){ 
        
        update();
        upload();
        
    };

} 

Node *UBO::add(Node* node) {

    Node::add(node);
    return node;

}

void UBO::editor() {

    // buffer.editor();

}

void UBO::destroy() { buffer.reset(); subscribers.resize(0); if (id) glDeleteBuffers(1, &id); } // delete name; ?

void UBO::update() {

    if (id) glDeleteBuffers(1, &id);
    
    glGenBuffers(1, &id);

    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferData(GL_UNIFORM_BUFFER, buffer.data.size(), NULL, GL_DYNAMIC_COPY);

    for (auto shader:subscribers) {

        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glUniformBlockBinding(shader->id, glGetUniformBlockIndex(shader->id, name.c_str()), binding);
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, id);

    }
    
}

void UBO::upload(){ upload(&buffer.data[0], buffer.data.size()); }

void UBO::upload(GLvoid* data, size_t size, GLuint offset){

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

// void UBO::fromJSON(){ 

//     rapidjson::Document json;
//     json.Parse(File("show/ubo.json").data.data());

//     if(json.HasParseError()) { std::cout << "ubo parse error" << std::endl; return; }

//     if (!json.HasMember("ubo_list")) { std::cout << "Missing ubo_list" << std::endl; return; }

//     for (auto &ubo : json["ubo_list"].GetArray()) { 

//         if (!ubo.HasMember("name")) { std::cout << "Missing ubo name" << std::endl; return; }
//         if (!ubo.HasMember("definition")) { std::cout << "Missing ubo definition" << std::endl; return; }

//         UBO* target = nullptr;
//         for (auto &existing_ubo:UBO::pool) { 
//             if (!strcmp(existing_ubo->name.c_str(),ubo["name"].GetString())) {
//                 target = existing_ubo;
//                 break;
//             }
//         }
//         if (!target) target = new UBO(ubo["name"].GetString());
        
//         for (auto &def : ubo["definition"].GetArray()) { 

//             if (!def.HasMember("name")) { std::cout << "Missing def name" << std::endl; return; }
//             if (!def.HasMember("quantity")) { std::cout << "Missing def quantity" << std::endl; return; }
//             if (!def.HasMember("components")) { std::cout << "Missing def definition" << std::endl; return; }

//             std::vector<Component*> components;
//             for (auto &comp : def["components"].GetArray()) { 

//                 if (!comp.HasMember("name")) { std::cout << "Missing comp name" << std::endl; return; }
//                 components.push_back(Component::id(comp["name"].GetString()));

//             }

//             target->addObject(def["name"].GetString(), components, def["quantity"].GetInt());

//         }

//     }

//     widget.updateUBOList();

// }

#include "include/vendor/rapidjson/prettywriter.h"

// void UBO::toJSON(){ 

//     using namespace rapidjson;

//     StringBuffer sb;
//     PrettyWriter<StringBuffer> writer(sb);

//     writer.StartObject();
//     writer.String("ubo_list");
//     writer.StartArray();

//     for (auto ubo : UBO::pool) { 

//         writer.StartObject();
//         writer.String("name");
//         writer.String(ubo->name.c_str());
//         writer.String("definition");
//         writer.StartArray();

//         for (auto def:ubo->definition) { 
            
//             writer.StartObject();
//             writer.String("name");
//             writer.String(def.name.c_str());
//             writer.String("components");
//             writer.StartArray();

//             for (auto comp:def.components) { 
                
//                 writer.StartObject();
//                 writer.String("name");
//                 writer.String(comp->name.c_str());

//                 writer.EndObject();

//             }

//             writer.EndArray();
//             writer.String("quantity");
//             writer.Int(def.quantity);
//             writer.EndObject();

//         }

//         writer.EndArray();
//         writer.EndObject();

//     }

//     writer.EndArray();
//     writer.EndObject();

//     File::write("show/ubo.json", sb.GetString());

// }