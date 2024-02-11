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
#include "file.hpp"
#include "vbo.hpp"
#include "shader.hpp"
#include "shaderfx.hpp"

void Nodes::init() {

    // ////////// xxx.HPP 

    // Ownr<xxx>::editor([](NODE* node, xxx *x){ });
    
    // Ownr<xxx>::oncreate([](NODE* node, xxx *x){ });

    // Ownr<xxx>::onadd<yyy>([](Node*_this,Node*node){ 

    //     auto s = ((Ptr<xxx>*)_this)->get();
    //     auto f = ((Ptr<yyy>*)node)->get();

    //     return node;
        
    // });

    ////////// FILE.HPP 

    Ownr<File>::oncreate([](NODE* node, File *file){ node->name = file->name+"."+file->extension; });

    Ownr<File>::editor([](NODE* node, File *file){ ImGui::Text(file->extension.c_str()); });

    ////////// UBO.HPP 

    Ownr<UBO>::oncreate([](NODE* node, UBO *ubo){ node->name = ubo->name; });

    Ownr<UBO>::editor([](NODE* node, UBO *ubo){ Ptr<Buffer>::editor_cbs[typeid(Buffer)](node, &ubo->buffer); });

    ////////// VBO.HPP 
    
    Ownr<VBO>::editor([](NODE*node,VBO*vbo){ Ptr<Buffer>::editor_cbs[typeid(Buffer)](node, &vbo->buffer); });

    ////////// STRUCT.HPP 

    Ownr<Struct>::oncreate([](NODE* node, Struct *s){ node->name = s->name; });

    Ownr<Struct>::editor([](NODE* node, Struct *s){ 
        
        ImGui::Text((node->name+" " +std::to_string(s->size)).c_str());

        for (auto& c : s->comps) {

            ImGui::Text((" - "+c->name+" "+std::to_string(c->size)).c_str());

            for (auto& m : c->members) {
                ImGui::Text(("  -- "+m.name+" "+std::to_string(m.size)).c_str());                
            }


        }
    
    });

    ////////// SHADER.HPP 

    Ownr<ShaderProgram>::editor([](NODE* node, ShaderProgram *shader){ 
        
        ImGui::Text(std::to_string(shader->loaded).c_str());
    
        ImGui::InputTextMultiline("frag shader", &shader->frag.src[0], shader->frag.src.length(), ImVec2(300,300));
        ImGui::InputTextMultiline("vert shader", &shader->vert.src[0], shader->vert.src.length(), ImVec2(300,300));
   
    });

    ////////// SHADERFX.HPP 

    Ownr<ShaderFX>::editor([](NODE* node, ShaderFX *shader){ 
            
        ImGui::InputTextMultiline("src", (char*)shader->file->data.c_str(), shader->file->data.size());
   
    });

    
    Ownr<ShaderFX>::oncreate([](NODE* node, ShaderFX *fx) { node->name = fx->file->name; });

    ////////// DRAWCALL.HPP 

    Ownr<DrawCall>::editor([](NODE* node, DrawCall *dc){ Ptr<ShaderProgram>::editor_cbs[typeid(ShaderProgram)](node, &dc->shader); });

    Ownr<DrawCall>::onadd<File>([](Node*_this,Node*node){ 
        
        auto dc = _this->is_a<DrawCall>();
        auto file = node->is_a<File>();
        if (!dc || !file) return node;

        auto model = dc->vbo.import(file);

        _this->Node::add(new Ptr<Model>(model));

        return node;

    });

    ////////// MODEL.HPP 

    Ownr<Model>::oncreate([](NODE* node, Model *model) { node->name = model->file->name; });

    Ownr<Model>::onadd<File>([](Node*_this,Node*node){ 
        
        auto model = _this->is_a<Model>();
        auto file = node->is_a<File>();
        if (!model || !file) return node;

        PLOGD << "uuuu";

        auto bad = new ShaderFX(file);

        model->addFX(bad);

        _this->Node::add(new Ptr<ShaderFX>(bad));
        
        auto dc = _this->parent()->is_a<DrawCall>();
        if (dc) dc->update();
        
        return node;

    });

    ////////// ENGINE.HPP (and Stack)

    Ownr<Stack>::onadd<DrawCall>([](Node*_this,Node*node){ 

        auto dc = ((Ptr<DrawCall>*)node)->get();
        dc->update();
    
        _this->Node::add(node);
        return _this; 
        
    });


    ////////// Directory.HPP 

    Ownr<Directory>::oncreate([](NODE* node, Directory *dir){ 

        node->name = dir->path;

        for (auto f : dir->list) ((Ownr<Directory>*)node)->add<File>(f);
        
    });

    ////////// BUFFER.HPP 

    Ownr<Buffer>::editor([](NODE* node, Buffer *buffer){

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