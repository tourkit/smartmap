#include "ubo.hpp"

UBO::~UBO() { destroy(); }

UBO::UBO(const char* name, std::vector<GLuint> subscribers) : name(name), subscribers(subscribers) { 
    
    pool.push_back(this); 

    widget.updateUBOList();
    
} 

void UBO::addStruct(const char* name, std::vector<Component*> components = {}, size_t quantity) {

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
    for (auto shader:subscribers) link(shader); 

}

void UBO::link(GLuint shader) {

    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glUniformBlockBinding(shader, glGetUniformBlockIndex(shader, name), binding);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, id);
}


void UBO::update(){ update(&data[0], 4*data.size()); }

void UBO::update(GLvoid* data, size_t size, GLuint offset){

    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data); 
}



void UBO::Widget::draw() {

    if (ImGui::Combo("UBO", &ubo_current, ubo_list.buffer)) updateStructList();

    ImGui::Spacing();

    if (!UBO::pool.size()) return;

    auto *ubo = UBO::pool[ubo_current];

    if (ubo->definition.size()) {

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
            
        }

        int uniform_offset = 0;
        ImGui::SliderInt("current##uibocurrent", &elem_current,0,def.quantity-1);
        
        ImGui::NewLine();

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

        int comp_current = 0;
        if (ImGui::Combo("##AddComponent", &comp_current, comp_list.buffer)) {

            if (comp_current) {
                
                def.components.push_back(Component::pool[comp_current-1]); 
                ubo->resize(); 
            }

        }
    }
    
    ImGui::NewLine();
    ImGui::InputText("##NewStruct", &struct_name[0], 60);
    ImGui::SameLine();
    if (ImGui::Button("Add Struct")) { ubo->addStruct(struct_name.c_str()); }

}
    