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


void Nodes::init() {

    // ////////// xxx.HPP 

    // NODE<xxx>::editor([](Node* node, xxx *x){ });
    
    // NODE<xxx>::oncreate([](Node* node, xxx *x){ });

    // NODE<xxx>::onadd<yyy>([](Node*_this,Node*node){ 

    //     auto s = ((Ptr<xxx>*)_this)->get();
    //     auto f = ((Ptr<yyy>*)node)->get();

    //     return node;
        
    // });

    ////////// FILE.HPP 

    NODE<File>::oncreate([](Node* node, File *file){ node->name = file->name+"."+file->extension; });

    NODE<File>::editor([](Node* node, File *file){ ImGui::Text(file->extension.c_str()); });

    ////////// UBO.HPP 

    NODE<UBO>::oncreate([](Node* node, UBO *ubo){ node->name = ubo->name; });

    NODE<UBO>::editor([](Node* node, UBO *ubo){ Ptr<Buffer>::editor_cbs[typeid(Buffer)](node, &ubo->buffer); });

    ////////// VBO.HPP 
    
    NODE<VBO>::editor([](Node*node,VBO*vbo){ });

    ////////// STRUCT.HPP 

    NODE<Struct>::oncreate([](Node* node, Struct *s){ 
        
        node->name = s->name;     
    
    });

    NODE<Struct>::editor([](Node* node, Struct *s){ 
        
        ImGui::Text((node->name+" " +std::to_string(s->size)).c_str());

        for (auto& c : s->comps) {

            ImGui::Text((" - "+c->name+" "+std::to_string(c->size)).c_str());

            for (auto& m : c->members) {
                ImGui::Text(("  -- "+m.name+" "+std::to_string(m.size)).c_str());                
            }


        }


    
    });

    NODE<Struct>::onadd<File>([](Node*_this,Node*node){ 

        auto s = ((Ptr<Struct>*)_this)->get();
        auto f = ((Ptr<File>*)node)->get();

        return node;
        
    });

    ////////// DRAWCALL.HPP 

    NODE<DrawCall>::editor([](Node* node, DrawCall *dc){ 
        
        ImGui::Text(std::to_string(dc->shader.loaded).c_str());
    
        ImGui::InputTextMultiline("frag shader", &dc->shader.frag.src[0], dc->shader.frag.src.length(), ImVec2(300,300));
        ImGui::InputTextMultiline("vert shader", &dc->shader.vert.src[0], dc->shader.vert.src.length(), ImVec2(300,300));
   
    });

    NODE<DrawCall>::onadd<File>([](Node*_this,Node*node){ 

        auto vbo = &((Ptr<DrawCall>*)_this)->get()->vbo;
        auto file = ((Ptr<File>*)node)->get();

        vbo->import(file);
    
        return node;
    });

    ////////// ENGINE.HPP (and Stack)
    
    NODE<Stack>::oncreate([](Node*node,Stack*stack){ node->name = "stack";    return node; });

    NODE<Stack>::onadd<DrawCall>([](Node*_this,Node*node){ 

        auto dc = ((Ptr<DrawCall>*)node)->get();
        dc->update();
        
        node->name = "drawcall";
        _this->Node::add(node);
        return _this; 
        
    });


    ////////// Directory.HPP 

    NODE<Directory>::oncreate([](Node* node, Directory *dir){ 

        node->name = dir->path; 
        for (auto f : dir->list) ((NODE<Directory>*)node)->add<File>(f);
        
    });

    ////////// BUFFER.HPP 

    NODE<Buffer>::oncreate([](Node* node, Buffer *buffer){ node->name = "buffy"; });

    NODE<Buffer>::editor([](Node* node, Buffer *buffer){


        static StringsBuffer object_str;
        static int obj_current = -1;
        static int elem_current = 0;
        static std::string uid = "123";
        std::vector<std::string> obect_strs;
        for (auto &obj : buffer->objects) obect_strs.push_back(obj.s->name);
        object_str.create(obect_strs);
        ImGui::Combo("Buffer##234sdfgsdfg", &obj_current, object_str.buffer);

        //// OBJ VIEW

        if (obj_current > -1) {
            
            auto &obj = buffer->objects[obj_current];

            if (ImGui::SliderInt("element##current", &elem_current, 0, obj.reserved)) {
                

                
            }


            if (!obj.reserved) return;

            int uniform_offset = obj.offset;

            for (auto c:obj.s->comps) {
                        
                // ImGui::Text((std::to_string(elem_current)+" - "+std::to_string(obj.size)).c_str());
                ImGui::SeparatorText(c->name.c_str());
                
                for (auto m:c->members) {

                    float *value = (float*)&buffer->data[uniform_offset+(elem_current*obj.s->size)];

                    if (ImGui::SliderFloat((m.name+"##"+c->name+m.name+uid+std::to_string(uniform_offset)).c_str(), value, m.range_from, m.range_to)) { 
                        
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