#pragma once

#include "pch.hpp"

#include "component.hpp"

struct Buffer {

    struct Object {

        std::string name;

        std::vector<Component*> components;

        int quantity,size = 0,offset = 0;
        
        // int get(int id, int comp, int member) { 
            
        //     return size*id; 
        
        // }

        void* buffer;

    };


    std::string name;

    std::vector<Object> objects;

    std::vector<char> data;  

    std::vector<Buffer*> pool;

    void add(Object *obj, std::vector<std::string> components) { 
        
        for (auto comp : components) { 
            
            obj->components.push_back(Component::id(comp.c_str())); 
            obj->size+=Component::id(comp.c_str())->size;
            
        } 

        resize(); 
        
    }

    // void remove(Object *obj, std::vector<Component*> components) {  for (auto comp : components) { obj->components.push_back(comp); } resize(); }

    void add(std::string name, std::vector<std::string> components = {}, int quantity = 1) { 

        objects.push_back({name, {}, quantity}); 

        add(&objects.back(),components); 

    }

    // template <typename T>
    // T &get(int id, int comp, int member) { 
        
    //     return data[]; 
    
    // }

    void reset() { objects.resize(0); data.resize(0); }

    void resize() {

        int size = 0;
        for (auto &obj:objects) {
            
            for (auto comp:obj.components) { size += comp->size*obj.quantity; }
            
        }

        data.resize(size);
        memset(&data[0],0,data.size());

        widget.updateBufferList();

    }


    struct Widget : GUI::Window {

        Buffer *buffer;
        int buffer_current = 0, object_current = 0, elem_current = 0;
        int add_comp;
        std::string add_buffer, add_object; 

        StringsBuffer object_list,buffer_list,comp_list;

        void updateBufferList() {

            std::vector<std::string> names;
            for (auto buffer:buffer->pool) { names.push_back(buffer->name); }
            buffer_list.create(names);
            
            if (buffer_current < buffer->pool.size()) updateObjectList(); 
            
        }

        void updateObjectList() {

            std::vector<std::string> names;
            for (auto &obj:buffer->pool[buffer_current]->objects) { names.push_back(obj.name); }
            object_list.create(names);

        }

        void updateCompList() {

            std::vector<std::string> names;
            for (auto &comp:Component::pool) { names.push_back(comp->name); }
            comp_list.create(names);

        }

        Widget(Buffer* buffer, std::string name = "Buffer") : GUI::Window(name) { 
            
            this->buffer = buffer;
            updateBufferList();
            add_buffer.resize(120);
            // update();

            updateCompList();
        
        }

        void draw() override {

            ImGui::InputText(("##New"+name).c_str(), &add_buffer[0], add_buffer.size());
            ImGui::SameLine();
            if (ImGui::Button(("Add "+name).c_str()) && strlen(&add_buffer[0])) { 
                
                new Buffer{add_buffer.c_str()};

                buffer_current = pool.size()-1;  
                
                memset(&add_buffer[0],0,add_buffer.size());

                // Buffer::toJSON();

                updateBufferList(); 
            
            }
            ImGui::Spacing();
            if (ImGui::Combo("list##234sdfgsdfg", &buffer_current, buffer_list.buffer)) updateObjectList();

            if (!buffer->pool.size()) return;

            ImGui::NewLine();

            ImGui::InputText("##NewObject", &add_object[0], add_object.size());
            ImGui::SameLine();
            if (ImGui::Button("Add Object") && strlen(&add_object[0])) { 
                
                buffer->add(add_object); 

                object_current = buffer->objects.size()-1;  
                
                memset(&add_object[0],0,add_object.size());

                UBO::toJSON();
            
            }
            ImGui::Spacing();

            if (!buffer->objects.size()) return;

            ImGui::Combo("object", &object_current, object_list.buffer);

            auto &obj = buffer->objects[object_current];
            
            if (ImGui::InputInt("quantity##objectquantity", &obj.quantity)) {
                
                if (!obj.quantity) {
                    
                    buffer->objects.erase(buffer->objects.begin()+object_current);
                    updateBufferList();
                    buffer->resize();        
                    // UBO::toJSON();
                    return;
                }

                buffer->resize(); 
                UBO::toJSON();
                
            }

            if (ImGui::SliderInt("current##uibocurrent", &elem_current,0,obj.quantity-1)) UBO::toJSON();
            
            ImGui::NewLine();
            
            ImGui::Combo("##AddComponent", &add_comp, comp_list.buffer);
            ImGui::SameLine();
            if (ImGui::Button("Add Component")) {

                obj.components.push_back(Component::pool[add_comp]); 
                buffer->resize(); 
                // UBO::toJSON();

            }

            ImGui::Spacing();

            ImGui::ShowDemoWindow();

            int members_size = 0;
            for (auto &c:obj.components) { members_size+= c->members.size(); }

            if (ImGui::BeginTable("items", obj.components.size(),  ImGuiTableFlags_BordersOuterH )) {

                for (auto &c:obj.components) {ImGui::TableSetupColumn(c->name.c_str()); }
                ImGui::TableHeadersRow();
                

                int comp_offset = 0;
                int members_offset = 0;

                for (auto &c:obj.components) {
                    ImGui::TableNextColumn();
                
                    if (ImGui::BeginTable("items", c->members.size(), ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
                    
                        for (auto &m:c->members) ImGui::TableSetupColumn(m.name.c_str()); 
                        ImGui::TableHeadersRow();

                        for (int id = 0; id < obj.quantity; id++) {
 
                            ImGui::TableNextRow(ImGuiTableRowFlags_None, 20);
                            for (int i = 0; i < c->members.size(); i++) {
                                
                                //nexttrow 
                                ImGui::TableNextColumn();
                                ;
                                ImGui::Text((std::to_string(members_offset)+ " + "+std::to_string(comp_offset)).c_str());
                                members_offset++;

                            }
                        }

                        ImGui::EndTable();
                        
                    }

                    comp_offset += c->size;

                }


            //     for (int row = 0; row < 5; row++)
            //     {
            //         ImGui::TableNextRow();
            //         for (int column = 0; column < 3; column++)
            //         {
            //             ImGui::TableSetColumnIndex(column);
            //             char buf[32];
            //             sprintf(buf, "Hello %d,%d", column, row);
            //             if (contents_type == CT_Text)
            //                 ImGui::TextUnformatted(buf);
            //             else if (contents_type == CT_FillButton)
            //                 ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
            //         }
            //     }
                ImGui::EndTable();
            }

            ImGui::Spacing();

            int uniform_offset = 0;
            for (auto c:obj.components) {
                
                ImGui::Text((std::to_string(elem_current)+" - "+std::to_string(obj.size)).c_str());
                if (ImGui::CollapsingHeader(c->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                
                    for (auto m:c->members) {

                        float *value = (float*)&buffer->data[uniform_offset+(elem_current*obj.size)];

                        if (ImGui::SliderFloat((m.name+"##"+c->name+m.name+uid).c_str(), value, m.range_from, m.range_to)) { 
                            
                            // ubo->update(); 
                        }

                        uniform_offset += m.size; 
                    }
                    
                } 
                   
                    // break; 
            }

            ImGui::NewLine();
            
            
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2,2));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));

            auto window_width = ImGui::GetWindowWidth();

            int cells_count = 48, cell_min = 0, cell_max = 255;
            int cell_width = window_width / cells_count - 2;

            for (int i = 0; i < buffer->data.size(); i++) {

                ImGui::PushID(i);

                if (ImGui::VSliderScalar("",  ImVec2(cell_width,30),    ImGuiDataType_U8, &buffer->data[i],  &cell_min,   &cell_max,   "")) { 
                    
                    // fixtureUBO->update(); 
                    
                }
                if ((i + 1) % cells_count != 0) { ImGui::SameLine(0); }

                ImGui::PopID();

            }

            ImGui::PopStyleVar(5);
            
            ImGui::NewLine();
        }

    } widget;

    Buffer(std::string name = "Buffer") : name(name), widget(this, name) { pool.push_back(this); }



};




// Buffer 

// [{

// struct 

//   de components
//   ...ETC
// [x tant]
// },]

// data mgmtW