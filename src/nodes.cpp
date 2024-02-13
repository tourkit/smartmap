#include "nodes.hpp"

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

void Nodes::init() {

    // ////////// xxx.HPP 

    // Node::editor<xxx>([](Node* node, xxx *x){ });
    
    // Node::oncreate<xxx>([](Node* node, xxx *x){ });

    // NODE<xxx>::onadd<yyy>([](Node*_this,Node*node){ 

    //     auto s = ((Ptr<xxx>*)_this)->get();
    //     auto f = ((Ptr<yyy>*)node)->get();

    //     return node;
        
    // });

    ////////// FILE.HPP 

    Node::oncreate<File>([](Node* node, File *file){ node->name = file->name+"."+file->extension+""; });

    static std::map<int,int> filechecks;

    Node::onrun<File>([](Node* node, File *file){ 

        int uid = 0;

        WIN32_FILE_ATTRIBUTE_DATA fileInfo; 
        
        GetFileAttributesExA(file->path.c_str(), GetFileExInfoStandard, &fileInfo);

        SYSTEMTIME st; 
        
        FileTimeToSystemTime(&fileInfo.ftLastWriteTime, &st);

        auto last = st.wMilliseconds;

        if (filechecks[uid] != last) { filechecks[uid] = last;  file->reload(); }

     });

    ////////// UBO.HPP 

    Node::onrun<UBO>([](Node* node, UBO *ubo){ ubo->upload(); });
    
    Node::oncreate<UBO>([](Node* node, UBO *ubo){ node->name = ubo->name; });

    Node::editor<UBO>([](Node* node, UBO *ubo){ Node::editor_cb<Buffer>(node, &ubo->buffer); });

    ////////// VBO.HPP 
    
    Node::editor<VBO>([](Node*node,VBO*vbo){ Node::editor_cb<Buffer>(node, &vbo->buffer); });

    ////////// STRUCT.HPP 

    Node::oncreate<Struct>([](Node* node, Struct *s){ node->name = s->name; });

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

    ////////// ENGINE.HPP (and Stack)

    NODE<Stack>::onadd<DrawCall>([](Node*_this,Node*node){ 

        auto dc = ((Ptr<DrawCall>*)node)->get();
        dc->update();
    
        _this->Node::add(node);
        return _this; 
        
    });

    NODE<Stack>::onadd<UBO>([](Node*_this,Node*node){ 

        _this->Node::add(node);
        return node;


    });

    ////////// DRAWCALL.HPP 

    Node::onrun<DrawCall>([](Node* node, DrawCall *dc){  dc->run(); });
    
    Node::editor<DrawCall>([](Node* node, DrawCall *dc){ Node::editor_cb<ShaderProgram>(node, &dc->shader); });

    NODE<DrawCall>::onadd<File>([](Node*_this,Node*node){ 
        
        auto dc = _this->is_a<DrawCall>();
        auto file = node->is_a<File>();
        if (!dc || !file) return node;

        auto model = dc->vbo.import(file);
        
        dc->update();

        return _this->Node::add(new Ptr<Model>(model));

    });

    ////////// MODEL.HPP 

    Node::oncreate<Model>([](Node* node, Model *model) { node->name = model->file->name; });

    NODE<Model>::onadd<File>([](Node*_this,Node*node){ 
        
        auto model = _this->is_a<Model>();
        auto file = node->is_a<File>();
        if (!model || !file) return node;

        auto bad = new ShaderFX(file); // unowned...

        model->addFX(bad);

        auto dc = _this->parent()->is_a<DrawCall>();
        if (dc) dc->update();
        
        return _this->Node::add(new Ptr<ShaderFX>(bad));

    });

    ////////// SHADERFX.HPP 

    Node::editor<ShaderFX>([](Node* node, ShaderFX *shader){ 
            
        ImGui::InputTextMultiline("src", (char*)shader->file->data.c_str(), shader->file->data.size());
   
    });

    
    Node::oncreate<ShaderFX>([](Node* node, ShaderFX *fx) { node->name = fx->file->name; });


    ////////// Directory.HPP 

    Node::oncreate<Directory>([](Node* node, Directory *dir){ 

        node->name = dir->path;

        for (auto f : dir->list) ((Ownr<Directory>*)node)->add<File>(f);
        
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

            if (ImGui::SliderInt("element##current", &elem_current, 0, obj.reserved-1)) {
                

                
            }

            ImGui::SameLine(); if (ImGui::Button("add")) (*buffer)[obj_current]->push();

            if (!obj.reserved) return;

            int uniform_offset = obj.offset;

            for (auto c:obj.s->comps) {
                        
                // ImGui::Text((std::to_string(elem_current)+" - "+std::to_string(obj.size)).c_str());
                ImGui::SeparatorText(c->name.c_str());
                
                for (auto m:c->members) {

                    auto name = (m.name+"##"+c->name+m.name+uid+std::to_string(uniform_offset)).c_str();

                    auto data = &buffer->data[uniform_offset+(elem_current*obj.s->size)];

                    auto type = ImGuiDataType_Float;

                    if (m.type == Component::Member::Type::UI8) type = ImGuiDataType_U8;
                    if (m.type == Component::Member::Type::UI16) type = ImGuiDataType_U16;
                    if (m.type == Component::Member::Type::UI32) type = ImGuiDataType_U16;

                    if (ImGui::SliderScalar(name, type, data, &m.range_from, &m.range_to)) { 
                        
                        // ubo->update(); 
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