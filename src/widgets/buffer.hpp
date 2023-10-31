#pragma once
#include "../gui.hpp"

  struct BufferWidget : GUI::Window {

        int buffer_current = -1, object_current = 0, elem_current = 0;

        static inline StringsBuffer buffers_list;
        StringsBuffer objects_list;
        
        std::string buffer_new, object_new; 
        int comp_new = -1;

        static void updateBufferList() {

            std::vector<std::string> names;
            for (auto *buffer : Buffer::pool) { names.push_back(buffer->name); }
            buffers_list.create(names);
            
        }

        void updateObjectList() {

            std::vector<std::string> names;
            for (auto &obj:Buffer::pool[buffer_current]->objects) { names.push_back(obj.name); }
            objects_list.create(names);
            object_current = 0;

        }

        BufferWidget(std::string name = "Buffer") : GUI::Window(name) { 
    
            updateBufferList();

            if (!Buffer::pool.size()) return;
            
            buffer_current = 0;
            updateObjectList();
        
        }

        void draw() override {

            // if (!Buffer::pool.size()) return;

            if (ImGui::Combo("Buffer##234sdfgsdfg", &buffer_current, buffers_list.buffer)) updateObjectList();

            if (buffer_current<0) return;

            Buffer *buffer =  Buffer::pool[buffer_current];

            ImGui::NewLine();

            ImGui::InputText("##NewObject", &object_new[0], object_new.size());
            ImGui::SameLine();
            if (ImGui::Button("Add Object") && strlen(&object_new[0])) { 
                
                buffer->add(object_new); 

                object_current = buffer->objects.size()-1;  
            
            }
            
            ImGui::Spacing();

            if (!buffer->objects.size()) return;

            ImGui::Combo("object", &object_current, objects_list.buffer);

            auto &obj = buffer->objects[object_current];
            
            if (ImGui::InputInt("quantity##objectquantity", &obj.quantity)) {
                
                // serait mieux genre changeObjQuantity
                
                if (!obj.quantity) {
                    
                    buffer->objects.erase(buffer->objects.begin()+object_current);
                    
                }

                buffer->updateBuffer();
                updateBufferList();
                // UBO::toJSON();
                
            }

            ImGui::Spacing();
            
            ImGui::Combo("##AddComponent", &comp_new, Component::buffer_string.buffer);
            ImGui::SameLine();
            if (ImGui::Button("Add Component")) {

                obj.addComponent(Component::pool[comp_new]->name); 

                buffer->updateBuffer();
                updateBufferList();
                
                // UBO::toJSON();

            }

            if (!obj.components.size()) return

            ImGui::NewLine();
            
            
            //////////////// SINGLE VIEW ////////////////////
            if (ImGui::CollapsingHeader("Single view")) {

                ImGui::NewLine();

                if (ImGui::SliderInt("current##uibocurrent", &elem_current,0,obj.quantity-1)) {}//UBO::toJSON();
            
                int uniform_offset = obj.buffer_offset;
                for (auto c:obj.components) {
                    
                    // ImGui::Text((std::to_string(elem_current)+" - "+std::to_string(obj.size)).c_str());
                    ImGui::SeparatorText(c->name.c_str());
                    
                    for (auto m:c->members) {

                        float *value = (float*)&buffer->data[uniform_offset+(elem_current*obj.byte_size)];

                        if (ImGui::SliderFloat((m.name+"##"+c->name+m.name+uid+std::to_string(uniform_offset)).c_str(), value, m.range_from, m.range_to)) { 
                            
                            // ubo->update(); 
                        }

                        uniform_offset += m.size; 
                    }
                    
                    
                        // break; 
                }
            }

            ImGui::NewLine();
            
            //////////////// TABLE VIEW ////////////////////
            if (ImGui::CollapsingHeader("Table view", ImGuiTreeNodeFlags_DefaultOpen)) {

                ImGui::NewLine();

                int members_size = 0;
                for (auto &c:obj.components) { members_size+= c->members.size(); }

                if (ImGui::BeginTable("items", obj.components.size(),   ImGuiTableFlags_Borders |  ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_NoPadInnerX )) {

                    for (auto &c:obj.components) {ImGui::TableSetupColumn(c->name.c_str()); }
                    ImGui::TableHeadersRow();

                    int comp_offset = 0;
                    int members_offset = obj.buffer_offset;
                    int col_members_offset = 0;

                    for (auto &c:obj.components) {

                        ImGui::TableNextColumn();
                    
                        if (ImGui::BeginTable("items", c->members.size(), ImGuiTableFlags_RowBg  | ImGuiTableFlags_BordersInner |  ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_NoPadInnerX )) {
                        
                            for (auto &m:c->members){

                                ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 10.0f);
                                
                                ImGui::TableSetupColumn(m.name.c_str()); 

                                ImGui::PopStyleVar();

                            }

                            ImGui::TableHeadersRow();

                            for (int id = 0; id < obj.quantity; id++) {
    
                                col_members_offset = 0;

                                ImGui::TableNextRow(ImGuiTableRowFlags_None, 20);
                                for (int i = 0; i < c->members.size(); i++) {
                                    
                                    ImGui::TableNextColumn();
                                    ImGui::SetNextItemWidth(-1);

                                    bool slider;
                                    if (c->members[i].type == Component::Member::Type::UI32) { 
                                        
                                        uint32_t min = 0, max = 32;
                                        const void *p_min = (void*)&min;
                                        const void *p_max = (void*)&max;
                                        slider = ImGui::SliderScalar(("##tableview"+std::to_string(id*obj.byte_size+members_offset+col_members_offset)).c_str(), ImGuiDataType_U32, (void*)&buffer->data[id*obj.byte_size+members_offset+col_members_offset], p_min, p_max , "%u"); 
                                        
                                    }
                                    else { 
                                        
                                        slider = ImGui::SliderFloat(("##tableview"+std::to_string(id*obj.byte_size+members_offset+col_members_offset)).c_str(), (float*)&buffer->data[id*obj.byte_size+members_offset+col_members_offset], 0,1 );
                                    
                                    }

                                    if (slider) buffer->callback();
                                        
                            
                                    col_members_offset+=c->members[i].size;

                                }

                            }

                            members_offset+=col_members_offset;

                            ImGui::EndTable();
                            
                        }

                        comp_offset += c->size;

                    }

                    ImGui::EndTable();

                }
            }

            ImGui::NewLine();

            if (ImGui::CollapsingHeader("Raw view")) {

                ImGui::NewLine();

                //////////////// RAW VIEW //////////////////////
                
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2,2));
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
                ImGui::SetWindowFontScale(.6666);

                auto window_width = ImGui::GetWindowWidth();

                int cells_count = 48, cell_min = 0, cell_max = 255;
                int cell_width = window_width / cells_count - 2;
                for (int i = 0; i < buffer->data.size(); i++) {

                    ImGui::PushID(i);

                    if (!(i%cells_count)) ImGui::NewLine();
                    ImGui::SameLine(((i%cells_count)*20)+8); 

                    ImGuiDataType_ datatype = ImGuiDataType_U8;

                    if (ImGui::VSliderScalar("",  ImVec2(cell_width,30),    datatype, &buffer->data[i],  &cell_min,   &cell_max,   "")) { 
                        
                        // fixtureUBO->update(); 
                        
                    }

                    ImGui::SameLine(0);
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - cell_width) ;
                    ImGui::Text(std::to_string(i).c_str());

                    ImGui::PopID();

                }
                ImGui::SetWindowFontScale(1);

                ImGui::PopStyleVar(5);
                
                ImGui::NewLine();
            
            }

        }


    } ;
    
    // static inline Buffer Buffer();