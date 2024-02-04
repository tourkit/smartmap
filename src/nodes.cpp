#include "nodes.hpp"
#include "node.hpp"
#include "gui.hpp"


#include "buffer.hpp"
#include "model.hpp"

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


    //////////////////////////////////////////////
    //////////////////////////////////////////////
    ////////// FILE.HPP 
    //////////////////////////////////////////////
    //////////////////////////////////////////////


    NODE<File>::onadd([](Node* node, File *file){ 

        node->name = file->name;
        
    });

    NODE<File>::editor([](Node* node, File *file){ 

        ImGui::Text(file->extension.c_str());

    });

    //////////////////////////////////////////////
    //////////////////////////////////////////////
    ////////// UBO.HPP 
    //////////////////////////////////////////////
    //////////////////////////////////////////////

    NODE<UBO>::editor([](Node* node, UBO *ubo){ 

        Ptr<Buffer>::editor_cbs[typeid(Buffer)](node, &ubo->buffer);
        
    });

    NODE<UBO>::onadd([](Node* node, UBO *ubo){ 

        node->name = ubo->name;
        
    });


    //////////////////////////////////////////////
    //////////////////////////////////////////////
    ////////// VBO.HPP 
    //////////////////////////////////////////////
    //////////////////////////////////////////////
    
    NODE<VBO>::onadd([](Node*node,VBO*vbo){ 

            return node;
    });
    
    NODE<VBO>::editor([](Node*node,VBO*vbo){ 

           ImGui::Text("ooooo");
    });

    NODE<VBO>::whitelist<File>([](Node*node,File*dc){ 
        
            PLOGD<< "oui";
        
        return nullptr;
        
    });


    //////////////////////////////////////////////
    //////////////////////////////////////////////
    ////////// DRAWCALL.HPP 
    //////////////////////////////////////////////
    //////////////////////////////////////////////

    NODE<DrawCall>::onadd([](Node* node, DrawCall *dc){ 

  
        dc->vbo = ((NODE<DrawCall>*)node)->add<VBO>()->get();

        return node;


    });


    NODE<DrawCall>::editor([](Node* node, DrawCall *dc){ 
    
        ImGui::InputTextMultiline("frag shader", &dc->shader.frag.src[0], dc->shader.frag.src.length(), ImVec2(300,300));
        ImGui::InputTextMultiline("vert shader", &dc->shader.vert.src[0], dc->shader.vert.src.length(), ImVec2(300,300));

        //imgui textbox
   
    });

    // NODE<DrawCall>::whitelist<File>([](Node*_this,Node*n){ 

    //     PLOGD<<"Zoo";
        
    //     DrawCall* dc = ((Ptr<DrawCall>*)_this)->get();

    //     File* file = ((Ptr<File>*)n)->get();

    //     dc->vbo.import(file);
        
    //     return nullptr; 
        
    // });


    //////////////////////////////////////////////
    //////////////////////////////////////////////
    ////////// ENGINE.HPP (and Stack)
    //////////////////////////////////////////////
    //////////////////////////////////////////////
    
    NODE<Stack>::whitelist<DrawCall>([](Node*node,DrawCall*dc){ 
        
        node->name = "drawcall";
        
        return node; 
        
    });


    //////////////////////////////////////////////
    //////////////////////////////////////////////
    ////////// Directory.HPP 
    //////////////////////////////////////////////
    //////////////////////////////////////////////

    NODE<Directory>::onadd([](Node* node, Directory *dir){ 

        node->name = dir->path; 
        for (auto f : dir->list) node->add(new Ptr<File>(f));
        
    });

    //////////////////////////////////////////////
    //////////////////////////////////////////////
    ////////// BUFFER.HPP 
    //////////////////////////////////////////////
    //////////////////////////////////////////////

    NODE<Buffer>::onadd([](Node* node, Buffer *buffer){ node->name = "buffy"; });

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
                
                
                    break; 
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

    });

    //////////////////////////////////////////////
    //////////////////////////////////////////////
    ////////// MODEL.HPP 
    //////////////////////////////////////////////
    //////////////////////////////////////////////

    NODE<Model>::onadd([](Node* node, Model *model){ node->name = model->name; });

    // NODE<Model>::whitelist<Foo>([](Node*_this,Node*foo){ return _this->Node::add(foo); });

    NODE<Model>::editor([](Node* node, Model *model){

        ImGui::Text("Posistiv");

    });



}