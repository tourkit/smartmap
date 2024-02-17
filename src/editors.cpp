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
#include "engine.hpp"

void Editors::init() {

    // ////////// xxx.HPP 

    // Editor::set<xxx>([](Node* node, xxx *x){ });

    ////////// UBO.HPP 

    Editor::set<UBO>([](Node* node, UBO *ubo){ 

        ImGui::Text((""+std::to_string(ubo->subscribers.size())+" subs").c_str());

        Editor::cb<Buffer>(node, ubo); 
        
    });

    ////////// VBO.HPP 
    
    Editor::set<VBO>([](Node*node,VBO*vbo){ Editor::cb<Buffer>(node, vbo); });

    ////////// STRUCT.HPP 

    Editor::set<Struct>([](Node* node, Struct *s){ 
        
        ImGui::Text((node->name+" " +std::to_string(s->size)).c_str());

        for (auto& c : s->comps) {

            ImGui::Text((" - "+c->name+" "+std::to_string(c->size)).c_str());

            for (auto& m : c->members) {
                ImGui::Text(("  -- "+m.name+" "+std::to_string(m.size)).c_str());                
            }


        }
    
    });

    ////////// SHADER.HPP 

    Editor::set<ShaderProgram>([](Node* node, ShaderProgram *shader){ 

        static TextEditor frageditor;
        static TextEditor verteditor;
        static bool init = false;
        if (!init) { 

            frageditor.SetShowWhitespaces(false);
            frageditor.SetReadOnly(false);
            frageditor.SetText(shader->frag.src); init = true; 
            verteditor.SetShowWhitespaces(false);
            verteditor.SetReadOnly(true);
            verteditor.SetText(shader->vert.src); init = true; 
            
        }
        
	    // auto lang = TextEditor::LanguageDefinition::CPlusPlus();
	    // frageditor.SetLanguageDefinition(lang);

        // frageditor.SetPalette(TextEditor::GetDarkPalette());
        
        frageditor.Render("frageditor");
        verteditor.Render("frageditor");
        
        if (frageditor.IsTextChanged()) {

            auto x = frageditor.GetText();

            memset(&x[x.size()-2],0,1);

            shader->create(x,shader->vert.src);

        }
   
    });

    ////////// DRAWCALL.HPP 
    
    Editor::set<DrawCall>([](Node* node, DrawCall *dc){ Editor::cb<ShaderProgram>(node, &dc->shader); });

    ////////// Log.HPP 
    
    Editor::set<Stack>([](Node* node, Stack *log){ 
        static File file(REPO_DIR+"logs.txt");
        file.reload();
        ImGui::Text(&file.data[0]); 
    });

    ////////// File.HPP 

    Editor::set<File>([](Node* node, File *file){ 

        char path[512]; 
        memset(path,0,512);
        memcpy(path,file->path.c_str(),file->path.size());

        if (ImGui::InputText("path", path, 512)) {
    
            if (strcmp(path, file->path.c_str())) {
                
                file->read(path);

                if (!file->loaded) node->name = "File";
                else node->name = node->name = file->name+"."+file->extension+"";
                
            }
        
        }
        
        char data[512000]; 
        memset(data,0,512000);
        memcpy(data,file->data.c_str(),file->data.size());

        if(ImGui::InputTextMultiline("src", data, 512000, ImVec2(600,300))) {

            if (strcmp(data, file->data.c_str())) {

                file->write(data); 

                node->update(); 
                
            }

        }

    });


    Editor::set<ShaderFX>([](Node* node, ShaderFX *shader){ 

        ImGui::InputTextMultiline("src", (char*)shader->file->data.c_str(), shader->file->data.size());
   
    });

    ////////// BUFFER.HPP 

    Editor::set<Buffer>([](Node* node, Buffer *buffer){

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

                node->update();

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

                        buffer->update();

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
                        
                buffer->update();
                
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