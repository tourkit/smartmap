#include "editor.hpp"

#include "log.hpp"
#include "node.hpp"
#include "gui.hpp"
#include "buffer.hpp"
#include "directory.hpp"
#include "component.hpp"
#include "struct.hpp"
#include "model.hpp"
#include "file.hpp"
#include "ubo.hpp"
#include "engine.hpp"
#include "drawcall.hpp"
#include "vbo.hpp"
#include "shader.hpp"
#include "effector.hpp"
#include "artnet.hpp"
#include "texture.hpp"
#include "atlas.hpp"
#include "json.hpp"

#include <cstring>

static int hovered_offset = -1;
static int hovered_size = -1;
static bool is_hovered = false;
namespace ImGui {

    static void TextX(const char* label, int offset, int size) {

        ImGui::Text(label);

        if (ImGui::IsItemHovered()) { 

            hovered_offset = offset;
            hovered_size = size;
            is_hovered=true;

        }
        // else{ 

        //     if (is_hovered){

        //         hovered_offset = -1;
        //         hovered_size = -1;
        //         is_hovered = false;

        //     }
        // }
        

    };

};

static void draw_definition(Buffer *buffer) {
            
    is_hovered=false;
    
    std::string str = "char[" +std::to_string( buffer->data.size()) + "]";

    ImGui::Text(str.c_str());

    for (auto &o: buffer->objects) {
        
        std::string str = "  "+o.s->name+"[" +std::to_string(o.s->size())+"]"+" * "+std::to_string(o.reserved);

        ImGui::TextX(str.c_str(), o.offset, o.s->size());

        int comp_offset = o.offset;
        for (auto& c : o.s->comps) {

            auto size = c->size;
            if (c->members.size() > 1 && o.s->is_striding) size = nextFactor(size,16);
            
            std::string str = "    "+c->name+"["+std::to_string(size)+"]";

            ImGui::TextX(str.c_str(), comp_offset, size);

            int member_offset = comp_offset;
            for (auto& m : c->members) {
                
                std::string str = "      "+(m.name.length()?m.name:"val")+"["+std::to_string(m.size)+"]";

                ImGui::TextX(str.c_str(), member_offset, m.size);
                
                member_offset+= m.size;
                
            }


            comp_offset+= size;

        }
                
        

        // maybe could merge two following thingyz
        auto x = o.s->size()-comp_offset;
        if (x) {
        
            std::string str = "      stride["+std::to_string(x)+"]";
            ImGui::TextX(str.c_str(), comp_offset, x);
        
        }
                
        //with that
        if (o.is_striding && o.stride()) { 

            std::string str = "    stride["+std::to_string(o.stride())+"]";
            ImGui::TextX(str.c_str(), comp_offset+x, o.stride());   

        }
        
    }

}

static void draw_raw(void *data, size_t size) {

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2,2));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
    
    ImGui::SetWindowFontScale(.8);

    auto window_width = ImGui::GetWindowWidth()-15;

    int cell_width = 20;
    int cell_margin = 1;

    int cells_per_line = (float)window_width/(cell_width+cell_margin);

    uint8_t ndata = 0;
    uint8_t cell_min = 0;
    uint8_t cell_max = 255;

    bool colorized = false;

    for (int i = 0; i < size; i++) {


        ImGui::PushID(i);
        if (is_hovered && i == hovered_offset) {

            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(.5,0,0,1));

            colorized = true;

        }

        if (!(i%cells_per_line)) ImGui::NewLine();
        ImGui::SameLine(((i%cells_per_line)*(cell_width+cell_margin))); 

        ImGuiDataType_ datatype = ImGuiDataType_U8;
        ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, ((*(((uint8_t*)data)+i))/255.0f)*26);

        if (ImGui::VSliderScalar("",  ImVec2(cell_width,30),    datatype, &ndata,  &cell_min,   &cell_max,   "")) { 
                    
            
        }
        
        ImGui::PopStyleVar(1);

        ImGui::SameLine(0);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - cell_width) ;
        std::string str = std::to_string(i);
        ImGui::Text(str.c_str());

        ImGui::PopID();
        if (colorized && (i == hovered_offset+hovered_size-1 || i == size-1)) { ImGui::PopStyleColor(); colorized= false; }

    }
    ImGui::SetWindowFontScale(1);

    ImGui::PopStyleVar(5);

}

static bool draw_object(void*data, Struct* s) {

    int uniform_offset = 0;

    bool has_changed = false;

    for (auto c:s->comps) {
                
        ImGui::SeparatorText(c->name.c_str());
        ImGui::Text("delete");
        if(ImGui::IsItemClicked()){
            s->removeComp(c->name);
            has_changed = true;
        }

        int uniform_member_offset = 0;
        
        for (auto m:c->members) {

            float *f = (float*)(((char*)data)+uniform_offset+uniform_member_offset);

            uniform_member_offset += m.size; 

            void* range_to = &m.range_to;

            static int range = 65000;

            auto type = ImGuiDataType_Float;

            if (m.type == Member::Type::I32) {type = ImGuiDataType_S32; range_to = &range; }
            if (m.type == Member::Type::UI8) type = ImGuiDataType_U8;
            if (m.type == Member::Type::UI16) type = ImGuiDataType_U16;
            if (m.type == Member::Type::UI32) { type = ImGuiDataType_U16; range_to = &range; }
            
            int q = 1;
            if (m.type == Member::Type::VEC2) q = 2;
            if (m.type == Member::Type::VEC3) q = 3;
            if (m.type == Member::Type::VEC4) q = 4;

            std::string name = (m.name+"##"+c->name+m.name+std::to_string(uniform_offset+uniform_member_offset));

            if (ImGui::SliderScalarN(name.c_str(), type, f, q, &m.range_from, range_to)) has_changed = true; 

        }

        
        uniform_offset += uniform_member_offset;
        
    }

    return has_changed;
            
 }


void Editors::init() {

    // ////////// xxx.HPP 

    // Editor<xxx>([](Node* node, xxx *x){ });

    ////////// Artnet.HPP 
    
    Editor<DMX::Remap>([](Node*node,DMX::Remap* remap){ 

            // string buffer for imgui::combo
            static std::vector<char> models;
            static std::vector<Model*> models_ptr;

            if (!models.size()) {
            
                engine.stack->each<Layer>([&](Node* n, Layer* l) { 

                    n->each<Model>([&](Node* n, Model* m) { 

                        for (auto c : n->name) models.push_back(c);
                        models.push_back(0);
                        models_ptr.push_back(m);
          
                    });

                    models.push_back(0);

                });

            }

            // imgui::combo
            static int model_id=0;
            if (ImGui::Combo("Model##dddddddd", &model_id, &models[0], models.size())) remap->import(models_ptr[model_id]->obj->s);

            std::string sss; 
            ImGui::InputText("char *src##remap_name", sss.data(), sss.length());

            int member_id = 0;

            for (auto c:remap->fixture->s->comps) {
                        
                ImGui::SeparatorText(c->name.c_str());
                
                for (auto m:c->members) {

                    ImGui::Text(m.name.c_str());

                    static int e = 0;
                    std::string strbypa = "bypass##rbt"+std::to_string(member_id);
                    ImGui::SameLine(); 
                    ImGui::RadioButton(strbypa.c_str(), &remap->fixture->attributes[member_id].combining , 0);
                    std::string strcoar = "coarse##rbt"+std::to_string(member_id);
                    ImGui::SameLine(); 
                    ImGui::RadioButton(strcoar.c_str(), &remap->fixture->attributes[member_id].combining , 1);
                    std::string strfine = "fine##rbt"+std::to_string(member_id);
                    ImGui::SameLine(); 
                    ImGui::RadioButton(strfine.c_str(), &remap->fixture->attributes[member_id].combining , 2);
                    std::string strultr = "ultra##rbt"+std::to_string(member_id);
                    ImGui::SameLine(); 
                    ImGui::RadioButton(strultr.c_str(), &remap->fixture->attributes[member_id].combining , 3);

                    member_id++;

                }
                
            }

    });

    Editor<Artnet>([](Node*node,Artnet* an){ 

        for (auto &u : an->universes) {

            std::string str = "universe "+std::to_string(u.first);
            ImGui::Text(str.c_str()); 

            draw_raw(&u.second->data[0], u.second->data.size());

        }

    });

    ////////// STRUCT.HPP 

    Editor<Struct>([](Node* node, Struct *s){ 
        
        std::string str = s->name+" " +std::to_string(s->size());
        ImGui::Text(str.c_str());

        for (auto& c : s->comps) {

            std::string str = " - "+c->name+" "+std::to_string(c->size);
            ImGui::Text(str.c_str());

            for (auto& m : c->members) {
                std::string str = "  -- "+m.name+" "+std::to_string(m.size);
                ImGui::Text(str.c_str());                
            }


        }
    
    });

    ////////// SHADER.HPP 

    Editor<ShaderProgram>([](Node* node, ShaderProgram *shader){ 

         ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) {

            if (ImGui::BeginTabItem("fragment")) {

                static TextEditor frageditor;

                static bool init = false;
                if (!init){

                    frageditor.SetShowWhitespaces(false);
                    frageditor.SetReadOnly(false);
                    frageditor.SetText(shader->frag.src); 

                    init = true;
                }

                static std::chrono::_V2::system_clock::time_point frag_last_change;
                if (frag_last_change != shader->last_change) { 

                    frageditor.SetText(shader->frag.src); 

                    frag_last_change = shader->last_change;

                }

                frageditor.Render("frageditor");

                if (frageditor.IsTextChanged()) {

                    auto x = frageditor.GetText();

                    memset(&x[frageditor.GetText().size()-2],0,1);

                    shader->create(x,shader->vert.src);

                }
                
                ImGui::EndTabItem();

            }

            if (ImGui::BeginTabItem("vertex")) {

                static TextEditor verteditor;

                static bool init = false;
                if (!init){

                    verteditor.SetShowWhitespaces(false);
                    verteditor.SetReadOnly(false);
                    verteditor.SetText(shader->vert.src); 

                    init = true;
                }

                static std::chrono::_V2::system_clock::time_point vert_last_change;
                if (vert_last_change != shader->last_change) { 

                    verteditor.SetText(shader->vert.src); 
                    vert_last_change = shader->last_change;
                    
                }
                
                verteditor.Render("frageditor");

                if (verteditor.IsTextChanged()) {

                    auto x = verteditor.GetText();

                    memset(&x[verteditor.GetText().size()],0,1);

                    shader->create(shader->frag.src,x);

                }

                ImGui::EndTabItem();

            }

            ImGui::EndTabBar();

        }
   
    });

    ////////// Log.HPP 
    
    Editor<Log>([](Node* node, Log *log_n){ 


        static bool is_verbose = false;

        static ImVec4 info = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
        static ImVec4 debug = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
        static ImVec4 warning = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
        static ImVec4 verbose = ImVec4(0.4f, 0.4f, 1.0f, 1.0f);

        static float *curr = &info.x;

        ImGui::SameLine();if(ImGui::ColorButton("info", info)){ curr = &info.x; ImGui::OpenPopup("picker");  }
        ImGui::SameLine();if (ImGui::ColorButton("debug", debug)) { curr = &debug.x; ImGui::OpenPopup("picker"); }
        ImGui::SameLine();if (ImGui::ColorButton("warning", warning)) { curr = &warning.x; ImGui::OpenPopup("picker"); }
        ImGui::SameLine();if (ImGui::ColorButton("verbose##vcolop", verbose)) { curr = &verbose.x; ImGui::OpenPopup("picker"); }
        ImGui::SameLine();ImGui::Checkbox("verbose", &is_verbose);

        if (ImGui::BeginPopup("picker")) { ImGui::ColorPicker4("#dfsdinfo", curr); ImGui::EndPopup(); }

        int max_lines = 1000;
        int to = log_n->appender.list.size()-max_lines;
        if (to<0)to = 0;
        for (int i = log_n->appender.list.size()-1; i>=to; i-- ) {
        // for (auto &m : log_n->appender.list ){

            auto &m = log_n->appender.list[i];

            ImVec4 color = info;

            if (m.severity == plog::Severity::debug) color = debug;
            if (m.severity == plog::Severity::warning) color = warning;
            if (m.severity == plog::Severity::verbose) {color = verbose;

                

                
            if (!is_verbose) continue;}

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            std::string str = " "+m.msg;
            ImGui::Text(str.c_str()); 
            ImGui::PopStyleColor();


        }
    });

    ////////// File.HPP 

    Editor<File>([](Node* node, File *file){ 


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
        memcpy(data,&file->data[0],file->data.size());

        static TextEditor codeeditor;
        static File *curr = nullptr;
        if (curr != file) {

            codeeditor.SetShowWhitespaces(false);
            codeeditor.SetReadOnly(false);
            codeeditor.SetText(file->data.data()); 
            curr = file;
        }
        
        codeeditor.Render("codeeditor");

        if (codeeditor.IsTextChanged()) {

            if (file->path.length()) file->write(codeeditor.GetText().c_str()); 

            else file->loadString(codeeditor.GetText().c_str());

            node->bkpupdate(); 

        }

    });

    ////////// OBJECT.HPP 

    Editor<Object>([](Node* node, Object *obj){

        ImGui::SeparatorText(obj->s->name.c_str());

        static std::unordered_map<Object*,int> elem_current;
        static std::string uid = "123";

        Buffer* buffer = obj->buffer;

        if (buffer->objects.size()) {

            int max = obj->reserved-1;
            int min = 0;
            if (max<0) min = -1;

            if (ImGui::SliderInt("instance##current", &elem_current[obj], min, max)) { }

            ImGui::SameLine(); if (ImGui::Button("add")) {

                obj->push();

                node->update();

            }

            if (!obj->reserved) return;

            if(draw_object(obj->data(elem_current[obj]),obj->s)) buffer->update();

        }

    });

    ////////// Texture.HPP 

    Editor<Texture>([](Node* node, Texture *texture){

        ImGui::InputScalarN("size",    ImGuiDataType_U32,  &texture->width, 2);

        float ratio = texture->height/(float)texture->width;
        auto nw = std::min(texture->width,(GLuint)512);

        ImGui::Image((void*)(uintptr_t)texture->id, ImVec2(nw,nw*ratio)); // pas comprendre implementation de ImTextureID (aka void*) , voir ImGui_ImplOpenGL3_RenderDrawData()

    });

    ////////// BUFFER.HPP 

    Editor<Buffer>([](Node* node, Buffer *buffer){

        ImGui::Separator();

        draw_definition(buffer);

        ImGui::Separator();

        draw_raw(buffer->data.data(),buffer->data.size());

        static StringsBuffer object_str;
        static int obj_current = 0;
        std::vector<std::string> obect_strs;
        for (auto &obj : buffer->objects) obect_strs.push_back(obj.s->name);
        if (!obect_strs.size()) return;
        object_str.create(obect_strs);
        ImGui::Combo("Buffer##234sdfgsdfg", &obj_current, object_str.buffer);

        if (obj_current <= buffer->objects.size()-1) Editor<Object>::cb(node, &buffer->objects[obj_current]);

    });
    
    Editor<Layer>([](Node* node, Layer *layer){ 
        
        Editor<Texture>::cb(node, layer->fb.texture); 

        Editor<ShaderProgram>::cb(node, &layer->shader); 

        ImGui::Separator();
        Editor<Object>::cb(node, layer->vbo.vertices); 
        ImGui::Separator();
        Editor<Object>::cb(node, layer->vbo.indices); 
        ImGui::Separator();
        Editor<VBO>::cb(node, &layer->vbo); 
        
    });

    ////////// UBO.HPP 

    Editor<UBO>([](Node* node, UBO *ubo){ 

        std::string subs_str = std::to_string(ubo->subscribers.size())+" sub(s):";
        for (auto s: ubo->subscribers) subs_str += " #"+std::to_string(s->id);
        ImGui::Text(subs_str.c_str());

        Editor<Buffer>::cb(node, ubo); 
        
    });

    ////////// MODEL.HPP 

    Editor<Model>([](Node* node, Model *model){ 
        
        if (node->parent()->is_a<Layer>()) Editor<Object>::cb(node, model->obj); 
        
        else if (model->file) Editor<File>::cb(node, model->file);
        
    });

    ////////// Artnet.HPP 
    
    Editor<DMX>([](Node*node,DMX* dmx){ 
        
        for (auto &r : dmx->remaps) Editor<DMX::Remap>::cb(node, &r); 
        
    });

    ////////// VBO.HPP 
    
    Editor<VBO>([](Node*node,VBO*vbo){ Editor<Buffer>::cb(node, vbo); });

    ////////// Effector.HPP 

    Editor<Effector>([](Node* node, Effector *effector){ 
  
        if (effector->file) Editor<File>::cb(node, effector->file); 
        
        for (auto x : effector->args)  { 
            
            std::string out_str = x.first+ " "+x.second;
            
            if (effector->ranges.find(x.second) != effector->ranges.end()) for (auto x : effector->ranges[x.second])  out_str += " " + std::to_string(x).substr(0,3) + ",";

            ImGui::Text(out_str.c_str());
        }
                
    });

    ////////// Engine.HPP 
    
    Editor<Stack>([](Node* node, Stack *stack){ Editor<Log>::cb(node, &logger); });
    
    Editor<Debug>([](Node* node, Debug *debug){ Editor<Log>::cb(node, &logger); });

    ////////// DRAWCALL.HPP 
    
    Editor<DrawCall>([](Node* node, DrawCall *dc){ Editor<ShaderProgram>::cb(node, &dc->shader); });

    ////////// Atlas.HPP 
    
    Editor<Atlas>([](Node* node, Atlas *atlas){ 
        
        Editor<Texture>::cb(node, atlas->texture); 
        
        Editor<Object>::cb(node, atlas->buffer); 
        
        
    });

    ////////// Component.HPP 
    
    Editor<Component>([](Node* node, Component *comp){ 
        
        for (auto &m : comp->members) {

            std::string str = std::string(m.type_name()) + " " + m.name+ "; ";
            ImGui::Text(str.c_str());
        }
        
        
    });
    ////////// JSON.HPP 
    
    Editor<JSON>([](Node* node, JSON *json){ 

        for (auto &m : (*json)["models"]) {ImGui::Text(m.name.GetString());ImGui::SameLine();ImGui::Text(m.value.GetString());}

        for (auto &m : (*json)["effectors"]) {ImGui::Text(m.name.GetString());ImGui::SameLine();ImGui::Text(m.value.GetString());}
            
        
    });

}



/////// EditorWidget.hpp

EditorWidget::EditorWidget() : GUI::Window("Editor")  {  }

void EditorWidget::draw() { 

    if (!selected || !locked) selected = engine.selected;

    std::string name = selected->name;
    // this->name = name;

    if (selected->parent()) { name = selected->parent()->name + "::" + name;  }

    ImGui::PushStyleColor(ImGuiCol_Text, selected->color);
    ImGui::Text("%s", name.c_str()); 
    ImGui::PopStyleColor(1);

    if (ImGui::IsItemClicked()) ImGui::OpenPopup("nodecolorpicker");
    if (ImGui::BeginPopup("nodecolorpicker")) { ImGui::ColorPicker4("#nodecolorpickercolor", &selected->color.x); ImGui::EndPopup(); }
    
    ImGui::SameLine(); ImGui::Checkbox("lock##locked", &locked);

    std::string referings;
    for (auto r : selected->referings) referings += " "+(r->name)+",";
    if (referings.length()) { 
        std::string str = "("+referings.substr(0,referings.length()-2)+")";
        ImGui::SameLine(); ImGui::Text(str.c_str()); }

    selected->editor();

}
