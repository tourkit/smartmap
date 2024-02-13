#include "editors.hpp"

#include "node.hpp"
#include "gui.hpp"
#include "buffer.hpp"
#include "directory.hpp"
#include "component.hpp"
#include "struct.hpp"
#include "file.hpp"
#include "ubo.hpp"
#include "engine.hpp"
#include "drawcall.hpp"
#include "vbo.hpp"
#include "shader.hpp"
#include "shaderfx.hpp"

void Editors::init() {

    // ////////// xxx.HPP 

    // Node::editor<xxx>([](Node* node, xxx *x){ });

    ////////// UBO.HPP 

    Node::editor<UBO>([](Node* node, UBO *ubo){ Node::editor_cb<Buffer>(node, &ubo->buffer); });

    ////////// VBO.HPP 
    
    Node::editor<VBO>([](Node*node,VBO*vbo){ Node::editor_cb<Buffer>(node, &vbo->buffer); });

    ////////// STRUCT.HPP 

    Node::editor<Struct>([](Node* node, Struct *s){ 
        
        ImGui::Text((node->name+" " +std::to_string(s->size)).c_str());

        for (auto& c : s->comps) {

            ImGui::Text((" - "+c->name+" "+std::to_string(c->size)).c_str());

            for (auto& m : c->members) {
                ImGui::Text(("  -- "+m.name+" "+std::to_string(m.size)).c_str());                
            }


        }
    
    });

    ////////// SHADER.HPP 

    Node::editor<ShaderProgram>([](Node* node, ShaderProgram *shader){ 
        
        ImGui::Text(std::to_string(shader->loaded).c_str());

        static TextEditor editor;
        
	    // auto lang = TextEditor::LanguageDefinition::CPlusPlus();
        
	    // editor.SetLanguageDefinition(lang);

        editor.SetText(shader->frag.src);
        editor.SetShowWhitespaces(false);

        // editor.SetPalette(TextEditor::GetDarkPalette());
        
        editor.Render("frageditor");
    
        // ImGui::InputTextMultiline("frag shader", &shader->frag.src[0], shader->frag.src.length(), ImVec2(600,300));
        // ImGui::InputTextMultiline("vert shader", &shader->vert.src[0], shader->vert.src.length(), ImVec2(600,300));
   
    });

    ////////// DRAWCALL.HPP 
    
    Node::editor<DrawCall>([](Node* node, DrawCall *dc){ Node::editor_cb<ShaderProgram>(node, &dc->shader); });

    ////////// SHADERFX.HPP 

    Node::editor<ShaderFX>([](Node* node, ShaderFX *shader){ 
            
        ImGui::InputTextMultiline("src", (char*)shader->file->data.c_str(), shader->file->data.size());
   
    });

    ////////// BUFFER.HPP 

    Node::editor<Buffer>([](Node* node, Buffer *buffer){

        static StringsBuffer object_str;
        static int obj_current = 0;
        static int elem_current = 0;
        static std::string uid = "123";
        std::vector<std::string> obect_strs;
        for (auto &obj : buffer->objects) obect_strs.push_back(obj.s->name);
        object_str.create(obect_strs);
        ImGui::Combo("Buffer##234sdfgsdfg", &obj_current, object_str.buffer);

        //// OBJ VIEW

        if (buffer->objects.size()) {
            
            auto &obj = buffer->objects[obj_current];

            int max = obj.reserved-1;
            if (max<0) max = 0;

            if (ImGui::SliderInt("element##current", &elem_current, 0, max)) {
                

                
            }

            ImGui::SameLine(); if (ImGui::Button("add")) {

                (*buffer)[obj_current]->push();

                if (buffer->owner) buffer->owner->update();

            }

            if (!obj.reserved) return;

            int uniform_offset = obj.offset;

            for (auto c:obj.s->comps) {
                        
                ImGui::SeparatorText(c->name.c_str());
                
                for (auto m:c->members) {

                    auto name = (m.name+"##"+c->name+m.name+uid+std::to_string(uniform_offset)).c_str();

                    auto data = &buffer->data[uniform_offset+(elem_current*obj.s->size)];

                    auto type = ImGuiDataType_Float;

                    if (m.type == Component::Member::Type::UI8) type = ImGuiDataType_U8;
                    if (m.type == Component::Member::Type::UI16) type = ImGuiDataType_U16;
                    if (m.type == Component::Member::Type::UI32) type = ImGuiDataType_U16;

                    if (ImGui::SliderScalar(name, type, data, &m.range_from, &m.range_to)) { 

                        if (buffer->owner) buffer->owner->upload();

                    }

                    

                    uniform_offset += m.size; 
                }
                
                
            }
            
        }

        //// RAW VIEW

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
        
        ImGui::Separator();

        for (auto &o:buffer->objects) {
            
            ImGui::Text((o.s->name+" " +std::to_string(o.s->size)).c_str());

            for (auto& c : o.s->comps) {

                ImGui::Text((" - "+c->name+" "+std::to_string(c->size)).c_str());

                for (auto& m : c->members) {
                    ImGui::Text(("  -- "+m.name+" "+std::to_string(m.size)).c_str());                
                }


            }
        }
    });

}