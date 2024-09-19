#include "editors.hpp"

#include "ImGuiColorTextEdit/TextEditor.h"
#include "imgui.h"
#include "rapidjson/stringbuffer.h"
#include "vendors/imgui/imgui_internal.h"

#include "editor.hpp"
#include "folder.hpp"
#include "output.hpp"
#include "ndi.hpp"
#include "model.hpp"
#include "file.hpp"
#include "ubo.hpp"
#include "engine.hpp"
#include "vbo.hpp"
#include "shader.hpp"
#include "effector.hpp"
#include "artnet.hpp"
#include "texture.hpp"
#include "atlas.hpp"
#include "framebuffer.hpp"
#include "json.hpp"
#include "layer.hpp"
#include "gui.hpp"

#include "log.hpp"
#include <ctype.h>
#include "tinyexpr/tinyexpr.h"
#include <map>

#include <format>
#include <sstream>
#include <string>

static int hovered_offset = -1;
static int hovered_size = -1;
static bool is_hovered = false;
static uint32_t is_hovered_winID = 0;



struct ImEditor {

    // static inline std::vector<const char*,bool> fasddsf;

    TextEditor editor;

    const char* src;

    bool has_changed = false;

    ImEditor() {

        editor.SetShowWhitespaces(false);

        editor.SetReadOnly(false);

    }

};


namespace ImGui {


    void CustomSliderScalarN(){


    }


};
namespace ImGui {

    static void TextX(std::string label, int offset, int size, int depth, std::vector<float> range = {}) {

        for (int i = 0; i < depth; i++) label = "    "+label;

        if (range.size()==2) label += " (" +std::format("{}",range[0])+", "+std::format("{}", range[1])+")";

        while (strlen(label.c_str()) < 35) label += " ";
        label += std::to_string(offset);
        while (strlen(label.c_str()) < 40) label += " ";
        label += std::to_string(size);

        ImGui::Text(label.c_str());

        if (ImGui::IsItemHovered()) {

            hovered_offset = offset;
            hovered_size = size;
            is_hovered=true;
            is_hovered_winID = ImGui::GetWindowDockID();

        }

    };

};

using namespace ImGui;

void draw_definition(Member *member_, int offset, int depth) {

    auto member = member_->ref();

    std::vector<float> range;

    if (member->isData()) {

        range.resize(2);

        float from = *(float*)member->from() ;
        float to = *(float*)member->to() ;
        size_t size = sizeof(float);

        if (member->type().id == typeid(int)) {

            from = *(int*)member->from();
            to = *(int*)member->to();
            size = sizeof(int);

        }
        if (member->type().id == typeid(char)) {

            from = *(char*)member->from();
            to = *(char*)member->to();
            size = sizeof(char);

        }

        memcpy(&range[0], &from, size);

        memcpy(&range[1], &to, size);

    }

    ImGui::TextX(std::string(!member->isData() ? "struct" : member->type_name()) + " " + member->name(), offset, member->footprint(), depth,range);

    for (auto m : member->members) {

        draw_definition(m, offset, depth+1);

        offset += m->footprint_all();
    }

    if (member->stride()) ImGui::TextX("float stride", offset, member->stride(), depth+1);

}

static bool draw_raw(void *data, size_t size) {

    bool has_changed = false;

    ImGui::NewLine();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2,2));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));

    ImGui::SetWindowFontScale(.8);

    auto window_width = ImGui::GetWindowWidth()-15;

    int cell_width = 24;
    int cell_margin = 1;

    int cells_per_line = (float)window_width/(cell_width+cell_margin);

    bool colorized = false;

    // ImGui::BeginDisabled();

         if (size > 1025) size = 1025;

        if (cells_per_line) for (int member_count = 0; member_count < size; member_count++) {


        ImGui::PushID(member_count);
        if (is_hovered_winID == ImGui::GetWindowDockID() && is_hovered && member_count == hovered_offset) {

            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(.5,0,0,1));

            colorized = true;

        }

        if (member_count && !(member_count%cells_per_line)) ImGui::NewLine();
        ImGui::SameLine(((member_count%cells_per_line)*(cell_width+cell_margin)));

        if (VCharSlider((((uint8_t*)data)+member_count),member_count)) has_changed = true;

        ImGui::PopID();
        if (colorized && (member_count == hovered_offset+hovered_size-1 || member_count == size-1)) { ImGui::PopStyleColor(); colorized= false; }

    }

    // ImGui::EndDisabled();

    ImGui::SetWindowFontScale(1);

    ImGui::PopStyleVar(5);

    static int all_at = 0;
    static std::vector<char> bkp;
    
    bool has_changed_all = false;
    
    if (Button("ALL @ 63###allat63"))  { all_at = 63; has_changed_all = true; }
    SameLine(); SetNextItemWidth(50);
    if (DragInt("##all_at",&all_at,1,0,255)) has_changed_all = true; 

    if (has_changed_all) {

        if (!bkp.size()){
            
            bkp.resize(size);

            memcpy(bkp.data(), data, size);
        
        }
        
        memset(data, all_at, size);

        has_changed = true;
        
    }

    if (bkp.size()) {
    
        SameLine();

        if (Button("reset##rereresset"))  { 

            memcpy(data, bkp.data(), size>bkp.size()?bkp.size():size);

            bkp.resize(0);

            has_changed = true;

        }

    }

    ImGui::NewLine();

    return has_changed;

}


static int MyResizeCallback(ImGuiInputTextCallbackData* data) {

    *(std::string*)data->UserData = data->Buf;

    return 0;

}

static bool draw_guis(Member* buff, Member* member, uint32_t offset, int& member_count) {

    
    if (!member)
        member = buff; 
    
    struct int_ { int val = 0; };
    static std::map<Member*,int_> elem_currents;
    int &elem_current = elem_currents[member].val;

    if (member->quantity() > 1 && member != buff) {

        SameLine();

        SetNextItemWidth(-FLT_MIN-10);
        if (ImGui::SliderInt(("##"+std::to_string(member_count++)).c_str(), &elem_current, 0, member->quantity()-1)) { }

        offset += member->footprint()*elem_current;

    }

    member = member->ref();

    bool has_changed = false;

    // if (member->ref()) member = member->ref();

    for (auto& m : member->members) {

        if (m->isData()) {

            auto type = ImGuiDataType_Float;

            if (m->type().id == typeid(int)) 
                type = ImGuiDataType_U32;

            std::string name = (m->name());


            static std::string str__;
            static std::string last_value;

            int q = m->quantity();


            // SetCursorPosX(85-CalcTextSize(name.c_str()).x);
            SetCursorPosX(5);

            SetNextItemWidth(-FLT_MIN-6);
            PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

            bool dis = false;
            bool blo = false;
            if (*(float*)m->to() == 0){
                ImGui::BeginDisabled();
                blo = true;
            }
            if (*(float*)m->to() == *(float*)m->from()){
                ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(1.0f, 0.0f, 0.0f, 0.0f));
                dis = true;
            }

            auto x = ImGui::SliderScalarN(("###"+std::to_string(member_count++)).c_str(), type, buff->data()+offset, q, m->from(), m->to(),NULL,0,
            ImGuiInputTextFlags_CallbackAlways|ImGuiInputTextFlags_EnterReturnsTrue, MyResizeCallback, &str__);
            
            if (dis)
                ImGui::PopStyleColor(1);

            if (blo)
                ImGui::EndDisabled();

            if (!(IsItemActive() && ImGui::TempInputIsActive(ImGui::GetCurrentContext()->LastItemData.ID))){
                
                SameLine(); 
                SetNextItemWidth(85);
                PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 1.0, 1.0, .65f));
                ImGui::SetCursorPosX(11);
                Text(name.c_str());
                PopStyleColor();
                
            }

            PopStyleColor();

            if (x>=0) {

                x*=4;

                static te_parser tep;
                if (str__.length() && !isdigit(str__.at(0)) && str__.at(0) != 46) {

                    if (last_value.length())str__=last_value+str__;

                    last_value = "";
                }

                double r = tep.evaluate(str__);

                str__.clear();

                if (!std::isnan(r)){

                    if (type == ImGuiDataType_Float) *(float*)(buff->data()+offset+x) = r;
                    else if (type == ImGuiDataType_S16) *(int16_t*)(buff->data()+offset+x) = r;
                    else if (type == ImGuiDataType_U16) *(uint16_t*)(buff->data()+offset+x) = r;

                }

                has_changed = true;
            }


            if (ImGui::IsItemClicked()) {

                    if (type == ImGuiDataType_Float) last_value = std::to_string(*(float*)(buff->data()+offset+(int)std::floor( ( ImGui::GetMousePos().x - ImGui::GetWindowPos().x ) / ImGui::GetItemRectSize().x * q )*4));
                    else if (type == ImGuiDataType_S16) last_value = std::to_string(*(int16_t*)(buff->data()+offset+(int)std::floor( ( ImGui::GetMousePos().x - ImGui::GetWindowPos().x ) / ImGui::GetItemRectSize().x * q )*4));
                    else if (type == ImGuiDataType_U16) last_value = std::to_string(*(uint16_t*)(buff->data()+offset+(int)std::floor( ( ImGui::GetMousePos().x - ImGui::GetWindowPos().x ) / ImGui::GetItemRectSize().x * q )*4));

            }


        }else{

            std::string septxt  = m->ref()->name();


            PushStyleColor(ImGuiCol_Text, ImVec4(0.6, 0.6, 0.6, 1.0f));
            ImGui::SeparatorText(septxt.c_str());
            PopStyleColor(1);

            if (draw_guis(buff, m, offset, member_count)) has_changed = true;

            // ImGui::Text("delete");
            // if(ImGui::IsItemClicked()){
            //     // s->remove(m->name()); // TOdoFIX
            //     has_changed = true;
            // }

        }

        offset+=m->footprint_all();

    }


    return has_changed;

 }



static bool IntButtons(int* p_data ) {

        bool value_changed = false;

        int p_step_ = 1;
        int p_step_fast_ = 100;
        void* p_step = (void*)&p_step_;
        void* p_step_fast = (void*)&p_step_fast_;

        const float button_size = GetFrameHeight();
        auto style = ImGui::GetStyle();
        BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
        PushID(engine.gui_v->member_count++);
        SetNextItemWidth(ImMax(1.0f, CalcItemWidth() - (button_size + style.ItemInnerSpacing.x) * 2));
        

        auto io = ImGui::GetIO();

        ImGuiInputTextFlags flags;
        
        const ImVec2 backup_frame_padding = style.FramePadding;
        style.FramePadding.x = style.FramePadding.y;
        ImGuiButtonFlags button_flags = ImGuiButtonFlags_Repeat | ImGuiButtonFlags_DontClosePopups;

        SameLine(0, style.ItemInnerSpacing.x);

        static std::set<int*> deletings;
        bool deleting = deletings.find(p_data)!=deletings.end();
        if (ButtonEx(deleting?"?":"-", ImVec2(button_size, button_size), button_flags)) {
            
            if (*p_data == 1 && !deleting) 
                deletings.insert(p_data);
                    
            else{

                deletings.erase(p_data);
            
                DataTypeApplyOp(ImGuiDataType_S32, '-', p_data, p_data, io.KeyCtrl && p_step_fast ? p_step_fast : p_step);
            
                value_changed = true;
                
            }
            
        }

        
        SameLine(0, style.ItemInnerSpacing.x);
        if (ButtonEx("+", ImVec2(button_size, button_size), button_flags))
        {
            deletings.erase(p_data);
            
            if (!deleting){
                
                DataTypeApplyOp(ImGuiDataType_S32, '+', p_data, p_data, io.KeyCtrl && p_step_fast ? p_step_fast : p_step);
                
                value_changed = true;
            
            }
        }

        style.FramePadding = backup_frame_padding;

        PopID();
        EndGroup();

        return value_changed;

}


void Editors::init() {

    // ////////// xxx.HPP

    // Editor<xxx>([](Node* node, xxx *x){ });

    ////////// Artnet.HPP



    Editor<NDI::Sender>([](Node*node, NDI::Sender* sender){



        Editor<Output>::cb( node, sender);


            ImGui::NewLine();
            ImGui::NewLine();

        if (sender->pixelcount) draw_raw( &sender->data[0], sender->pixelcount );


    });

    Editor<DMXRemap>([](Node*node, DMXRemap* remap){


        ImGui::SetNextItemWidth(100);
        ImGui::InputInt("channel##chjdshjkers", &remap->chan);

        ImGui::SetNextItemWidth(500);
        if (ImGui::BeginTable("##remapswindow", 7, ImGuiTableFlags_Borders, ImVec2(550, 0))) {

            ImGui::TableSetupColumn("name");
            ImGui::TableSetupColumn("skip", ImGuiTableColumnFlags_WidthFixed, 40);
            ImGui::TableSetupColumn("coarse", ImGuiTableColumnFlags_WidthFixed, 40);
            ImGui::TableSetupColumn("fine", ImGuiTableColumnFlags_WidthFixed, 40);
            ImGui::TableSetupColumn("ultra", ImGuiTableColumnFlags_WidthFixed, 40);
            ImGui::TableSetupColumn("min/max", ImGuiTableColumnFlags_WidthFixed, 120);
            ImGui::TableSetupColumn("active", ImGuiTableColumnFlags_WidthFixed, 40);
            ImGui::TableHeadersRow();

            int member_id = 0;

            Instance(*remap->dst.stl.back().m).each([&](Instance inst) {

                auto *m = inst.stl.back().m;

                 if (!m->isData()) return;

                    ImGui::TableNextRow();

                    // if (ImGui::IsItemHovered()) ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(.5,.5,.5,1));

                    ImGui::TableNextColumn();
                    // auto textWidth   = 100-ImGui::CalcTextSize(m->name().c_str()).x;
                    // ImGui::SetCursorPosX((textWidth * 0.5f) );

                    std::string name = m->name();
                    if (inst.stl.size()>1) name = inst.stl[inst.stl.size()-2].m->name()+"."+name;

                    ImGui::Text(name.c_str());

                    if (member_id < remap->attributes.size()) {

                        static int e = 0;

                        bool changed = false;

                        ImGui::TableNextColumn();
                        if (ImGui::RadioButton(("##arbt"+std::to_string(member_id)).c_str(), &remap->attributes[member_id].combining , 0)) 
                            changed = true;
                        ImGui::TableNextColumn();
                        if (ImGui::RadioButton(("##vrbt"+std::to_string(member_id)).c_str(), &remap->attributes[member_id].combining , 1)) 
                            changed = true;
                        ImGui::TableNextColumn();
                        if (ImGui::RadioButton(("##xrbt"+std::to_string(member_id)).c_str(), &remap->attributes[member_id].combining , 2)) 
                            changed = true;
                        ImGui::TableNextColumn();
                        if (ImGui::RadioButton(("##srbt"+std::to_string(member_id)).c_str(), &remap->attributes[member_id].combining , 3)) 
                            changed = true;
                        ImGui::TableNextColumn();
                        ImGui::SetNextItemWidth(120);
                        if (ImGui::InputFloat2(("##minmax"+std::to_string(member_id)).c_str(), &remap->attributes[member_id].min)) 
                            changed = true;
                        ImGui::TableNextColumn();
                        if (ImGui::Checkbox(("##arsbt"+std::to_string(member_id)).c_str(), &remap->attributes[member_id].active)) 
                            changed = true;

                        if (changed) {
                            for (int i = 1; i < m->quantity(); i++) {
                                remap->attributes[member_id+i].min = remap->attributes[member_id].min;
                                remap->attributes[member_id+i].max = remap->attributes[member_id].max;
                                remap->attributes[member_id+i].combining = remap->attributes[member_id].combining;
                                remap->attributes[member_id+i].active = remap->attributes[member_id].active;
                            }
                        }

                    }

                    member_id+=m->quantity();

                    // if (ImGui::IsItemHovered()) ImGui::PopStyleColor();


                

            });

                ImGui::EndTable();
            }

        std::stringstream deststart;
        deststart  <<  (void*)remap->dst.data();
        deststart  << " - " << (void*)remap->dst.stl.front().m->data();
        deststart  << " - " << remap->dst.offset;
        ImGui::Text(deststart.str().c_str()) ;

        std::stringstream buffa;
        buffa << remap->src.stl.back().m->name() << " - " << &remap->src;
        std::stringstream buffb;
        buffb << remap->dst.stl.back().m->name() << " - " << &remap->dst;

        ImGui::NewLine(); ImGui::SetNextItemWidth( ImGui::CalcTextSize( (char*)buffa.str().c_str() ).x); ImGui::InputText("##puppyaaa", (char*)buffa.str().c_str(), 10, ImGuiInputTextFlags_ReadOnly);

        ImGui::SameLine(); ImGui::Text(" -> ");

        ImGui::SameLine(); ImGui::SetNextItemWidth( ImGui::CalcTextSize( (char*)buffb.str().c_str() ).x); ImGui::InputText("##puppybbb", (char*)buffb.str().c_str(), 10, ImGuiInputTextFlags_ReadOnly);

        ImGui::SameLine(); ImGui::Text(("* "+std::to_string(remap->dst.stl.back().m->quantity())).c_str());

        // engine.
    });

    Editor<Artnet>([](Node*node,Artnet* an){

        ImGui::Text(an->name().c_str());

        ImGuiTextBuffer tbuff;
        for (auto x : an->available_ips) tbuff.append(x.c_str(), x.c_str()+x.length()+1);

        if (Combo("device##available_ips", &an->device_id, tbuff.begin())) {

            node->active(false);
            an->connect(an->available_ips[an->device_id]);

        }

        for (auto x : an->universes) {
            std::string str;


            auto inst = Instance(*an)[&x.second->m];

            str = "universe "+std::to_string(x.first) + " " + std::to_string(inst.offset) + " " + x.second->m.name();
            ImGui::Text(str.c_str());

            ImGui::NewLine();
            if (draw_raw(an->data()+(*x.second->m.instances.begin())->offset, 512)) {

                node->update();

            }
            ImGui::NewLine();

        };

    });


    ////////// WINDOW.HPP

    Editor<Window>([](Node* node, Window *window){

        using namespace ImGui;

        Editor<Output>::cb( node, window);


        NewLine();

        int i = 0;

        for (auto &display : Window::displays) {

            if (Button( ("fit display "+std::to_string(i++)).c_str())) {

                window->pos( display.x, display.y );
                window->size( display.width, display.height );

            }
            SameLine();
        }
        NewLine();
        NewLine();


        if (window->shader) Editor<ShaderProgram>::cb( node, window->shader);

    });

    ////////// OUTPUT.HPP

    Editor<Output>([](Node* node, Output *output){

        static std::map<Output*,int> output_currents; 
        static int min0 = 0; 
        static int min1 = 1; 
        ImGuiTextBuffer tbuff;

        for (auto x : engine.main->childrens) tbuff.append(x->name().c_str(), x->name().c_str()+x->name().length()+1);

        if (tbuff.size()) if (ImGui::Combo("source", &output_currents[output], tbuff.begin())) {

            engine.main->each<Layer>([&](Node* n, Layer* layer){

                if (layer && &layer->fb == output->fb) n->referings.erase(node);

            });
            
            output->fb = &engine.main->childrens[output_currents[output]]->is_a<Layer>()->fb;

            engine.main->childrens[output_currents[output]]->referings.insert(node);
            
        }

        if (ImGui::DragScalarN("position###winpos", ImGuiDataType_S32,  &output->offset_x, 2, 1, &min0)) 
            output->pos( output->offset_x, output->offset_y );

        if (ImGui::DragScalarN("size###winsize", ImGuiDataType_U32,  &output->width, 2, 1, &min1)) 
            output->size( output->width, output->height );

    });

    ////////// SHADER.HPP


    Editor<ShaderProgram>([](Node* node, ShaderProgram *shader){

        if (ImGui::BeginPopupContextItem()){

            if (ImGui::Button("create")) {
                // builder->build();
                // shader.create(builder->frag(), builder->vert());
                node->update();
            }
            if (ImGui::Button("empty")) { 

                static Builder BUILDER;
                Layer* lay = node->is_a<Layer>(); 
                if (lay) 
                    BUILDER.vbo = &lay->vbo; 
                shader->destroy(); 
                BUILDER.build(shader);
                shader->create(BUILDER.frag(), BUILDER.vert()); 
            }
            
            EndPopup();
        
        }

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;


        static TextEditor editor;

        static bool init = false;


        if (!init){

            editor.SetShowWhitespaces(false);

            editor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
            editor.SetReadOnly(false);
            
            init = true;
        }


        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) {

            if (ImGui::BeginTabItem("fragment")) {


                if (editor.GetText().length() != shader->frag.src.length()+1) {
                    
                    editor.SetText(shader->frag.src);
                    editor.SetErrorMarkers(shader->frag.errors);
                    node->error = shader->frag.errors.size();

                }

                
                editor.Render("fragment");

                if (editor.IsTextChanged()) {
                    
                    std::string x = editor.GetText();

                    memset(&x[x.length()-1],0,1); // against new line everytime dafuk
                    shader->destroy(); 
                    shader->create(x,shader->vert.src);

                    editor.SetErrorMarkers(shader->frag.errors);
                    node->error = shader->frag.errors.size();

                    

                }
                

                ImGui::EndTabItem();

            }
            if (ImGui::BeginTabItem("vertex")) {

                if (editor.GetText().length() != shader->vert.src.length()+1) {
                
                    editor.SetText(shader->vert.src);
                    editor.SetErrorMarkers(shader->vert.errors);
                    node->error = shader->vert.errors.size();
              
                }

                editor.Render("vertex");

                if (editor.IsTextChanged()) {

                    std::string x = editor.GetText();

                    memset(&x[x.length()-1],0,1); // against new line everytime dafuk
                    shader->destroy(); 
                    shader->create(shader->frag.src,x);

                    editor.SetErrorMarkers(shader->vert.errors);
                    node->error = shader->vert.errors.size();
                    
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
        static ImVec4 warning = ImVec4(1.0f, 0.7f, 0.0f, 1.0f);
        static ImVec4 error = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
        static ImVec4 verbose = ImVec4(0.4f, 0.4f, 1.0f, 1.0f);

        static float *curr = &info.x;

        if(ImGui::ColorButton("info", info)){ curr = &info.x; ImGui::OpenPopup("picker");  }
        ImGui::SameLine();if (ImGui::ColorButton("debug", debug)) { curr = &debug.x; ImGui::OpenPopup("picker"); }
        ImGui::SameLine();if (ImGui::ColorButton("warning", warning)) { curr = &warning.x; ImGui::OpenPopup("picker"); }
        ImGui::SameLine();if (ImGui::ColorButton("error", error)) { curr = &error.x; ImGui::OpenPopup("picker"); }
        ImGui::SameLine();if (ImGui::ColorButton("verbose##vcolop", verbose)) { curr = &verbose.x; ImGui::OpenPopup("picker"); }
        ImGui::SameLine();ImGui::Checkbox("verbose", &is_verbose);
        ImGui::SameLine(); if (ImGui::Button("clear")) { log_n->appender.list.resize(0); }

        if (ImGui::BeginPopup("picker")) { ImGui::ColorPicker4("#dfsdinfo", curr); ImGui::EndPopup(); }

        int max_lines = 1000;
        int count = 0;

        for (int member_count = log_n->appender.list.size()-1; member_count>=0; member_count-- ) {

            auto &m = log_n->appender.list[member_count];

            ImVec4 color = info;

            if (m.severity == plog::Severity::debug) color = debug;
            if (m.severity == plog::Severity::warning) color = warning;
            if (m.severity == plog::Severity::error) color = error;
            if (m.severity == plog::Severity::verbose) {color = verbose; if (!is_verbose) { continue;} }

            count++;
            if (count == max_lines) break;

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            std::string str;
            tm t;
            plog::util::localtime_s(&t, &m.time.time);
            plog::util::nostringstream ss;
            ss <<std::setfill('0') << std::setw(4) << static_cast<int> (m.id) << ":"<< std::setfill('0') << std::setw(2)<<t.tm_sec<<"."<< std::setfill('0') << std::setw(3) << static_cast<int> (m.time.millitm);

            str+="[";
            str+=ss.str();
            str+="] ";

            str += m.msg;

            int x = 130;
            ImGui::TextEx(str.c_str(), str.c_str()+(str.length()>x?x:str.length())); if (str.length()>x) { ImGui::SameLine(); ImGui::Text("..."); }
            ImGui::PopStyleColor();

            if (ImGui::BeginPopupContextItem(("##dsfgsdf"+std::to_string(count)).c_str())) {

                if (ImGui::Button("copy" )) { ImGui::SetClipboardText(str.c_str()); ImGui::CloseCurrentPopup(); }

                ImGui::EndPopup();

            }
        }
    });

    ////////// File.HPP

    Editor<File>([](Node* node, File *file){

        if (file->extension == "obj") {

            if (ImGui::Button("FBX")) Model::convert(file, "fbx"); ImGui::SameLine();
            if (ImGui::Button("3DS")) Model::convert(file, "3ds"); ImGui::SameLine();
            if (ImGui::Button("DXF")) Model::convert(file, "dxf"); ImGui::SameLine();

        }

        char path[512];
        memset(path,0,512);
        memcpy(path,file->path_v.c_str(),file->path_v.size());

        if (ImGui::InputText("path", path, 512)) {

            if (strcmp(path, file->path_v.c_str())) {

                file->read(path);

                if (!file->loaded) node->name("File");
                else node->name(file->name()+"."+file->extension+"");

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

            if (!file->owned) file->write(codeeditor.GetText().c_str());

            else file->loadString(codeeditor.GetText().c_str());

            node->update();

        }

    });


    ////////// Texture.HPP

    Editor<Texture>([](Node* node, Texture *texture){

        // if (ImGui::InputScalarN("size",    ImGuiDataType_U32,  &texture->width, 2) ) { texture->create( texture->width, texture->height, texture->unit, texture->mipmaps, texture->informat, texture->outformat ); }
        Layer* layer = node->is_a_nowarning<Layer>();

        static int p_min = 1;
        if (ImGui::DragScalarN("size",    ImGuiDataType_U32,  &texture->width, 2, 1, &p_min) && layer) { 
            
    logger.cout(Sev::verbose);
            node->update(); 
    logger.cout(Sev::warning);
        
        }

        float ratio = texture->height/(float)texture->width;
        auto nw = std::min(texture->width,(GLuint)512);

        ImGui::Image((void*)(uintptr_t)texture->id, ImVec2(nw,nw*ratio)); // pas comprendre implementation de ImTextureID (aka void*) , voir ImGui_ImplOpenGL3_RenderDrawData()

    });

    ////////// HPP

    Editor<Member>([](Node* node, Member *m){


        // std::stringstream ss;
        // ss << m;
        // ImGui::Text(( ss.str()).c_str());
         if (ImGui::BeginTabBar("def", ImGuiTabBarFlags_None)) {


            if (ImGui::BeginTabItem("programmer")) {
                
                if (m->buffering() && draw_guis(m,nullptr,0,engine.gui_v->member_count)) {
            
                    m->upload();

                    // engine.stack->each<UberLayer>([](Node*n, UberLayer* ubl){ ubl ->fb.clear();});
                    // engine.stack->each<Layer>([](Node*n, Layer* layer){ layer ->fb.clear();});

                    // engine.stack->each([](Node* node){ if (node->type().id == typeid(UberLayer) || node->type().id == typeid(Layer)) ((Layer*)node->ptr)->fb.clear(); });

                }

                ImGui::EndTabItem();

            }
            if (ImGui::BeginTabItem("definition")) {
                

                draw_definition(m);

                ImGui::Separator();

                if (m->buffering() && draw_raw(m->data(),m->footprint())) m->upload();

                ImGui::Separator();

                ImGui::EndTabItem();

            }
                
            ImGui::EndTabBar();

        }



    });


    ////////// UBO.HPP

    Editor<UBO>([](Node* node, UBO *ubo){

        // std::string subs_str = "data[" + std::to_string(ubo->data.size())+"]";
        // std::string subs_str = std::to_string(ubo->subscribers.size())+" sub(s):";
        // for (auto s: ubo->subscribers) subs_str += " #"+std::to_string(s->id);
        // ImGui::Text(subs_str.c_str());

        Editor<Member>::cb( node, ubo);

    });

    ////////// MODEL.HPP

    Editor<Effectable>([](Node* node, Effectable *effectable){

        static std::map<Effectable*,int> effector_currents;

        if (!effectable->instance)
            return;

        effector_currents[effectable] = effectable->quantity();

        // Separator();

        ImGui::SeparatorText(effectable->name().c_str());

        SameLine();

        SetNextItemWidth(150);
        if (IntButtons(&effector_currents[effectable])) { 

            if (!effector_currents[effectable]) {
               
               engine.gui_v->delete_list.push_back(node);
               
                
            }else{
    
                effectable->quantity(effector_currents[effectable]); 
            
                node->update(); 
            
            }

        }

        if (draw_guis(engine.dynamic_ubo, effectable, effectable->instance->offset,engine.gui_v->member_count))
            engine.dynamic_ubo->upload();

    });

    ////////// Artnet.HPP
    ////////// Artnet.HPP

    // Editor<Universe>([](Node*node,Universe* dmx){



    //     for (auto &r : dmx->remaps) Editor<Universe::Remap>::cb( node, &r);

    // });

    ////////// VBO.HPP

    Editor<VBO>([](Node*node,VBO*vbo){


        if (ImGui::BeginPopupContextItem()){

            std::ostringstream address;
            address << (void const *)vbo;
            ImGui::Text(("VBO " + std::to_string(vbo->vbo) + " - IBO " + std::to_string(vbo->ibo) + " - VAO " + std::to_string(vbo->vao) + " - " +  address.str()).c_str());

            if (ImGui::Button("destroy")) vbo->deleteData();
            if (ImGui::Button("create")) vbo->create();
            if (ImGui::Button("reset")) vbo->reset();
            if (ImGui::Button("addQuad")) vbo->addQuad();
            if (ImGui::Button("glDeleteVertexArrays")) glDeleteVertexArrays(1, &vbo->vao);
            
            EndPopup();
        
        }

        Editor<Member>::cb( node, vbo);

    });

  /////////////////////////////////


    ////////// Atlas.HPP

    Editor<Atlas>([](Node* node, Atlas *atlas){

        if (atlas) Editor<Texture>::cb( node, atlas->texture);
        else PLOGE << "NONONON";

        // Editor<Object>::cb( node, atlas->m);  // tofix


    });

    ////////// Component.HPP

    // Editor<Component>([](Node* node, Component *comp){

    //     for (auto &m : comp->members) {

    //         std::string str = std::string(m.type_name()) + " " + m.name+ "; ";
    //         ImGui::Text(str.c_str());
    //     }


    // });
    ////////// JSON.HPP

    Editor<JSON>([](Node* node, JSON *json){

        JSON::if_obj_in("models",json->document, [&](auto &m) { ImGui::Text(m.name.GetString());ImGui::SameLine();ImGui::Text(m.value.GetString()); });
        JSON::if_obj_in("effectors",json->document, [&](auto &m) { ImGui::Text(m.name.GetString());ImGui::SameLine();ImGui::Text(m.value.GetString()); });

    });
    ////////// FRAMEHPP

    Editor<FrameBuffer>([](Node* node, FrameBuffer *fb ){

        // ImGui::Text(("attachment "+std::to_string(fb->attachments)).c_str());

        Editor<Texture>::cb( node, &fb->texture);

    });

      ////////// Engine.HPP
    
    Editor<Stack>([](Node*node,Stack* stack){


        ImVec2 btn_size = {100,50};
        if (ImGui::Button("layer", btn_size)) {
            node->addOwnr<Layer>();
            
        }
        ImGui::SameLine();
        if (ImGui::Button("uberlayer", btn_size)) {

            node->addOwnr<UberLayer>();

        }

    });

    Editor<Debug>([](Node* node, Debug *debug){

        // Separator();

        // static std::vector<GLenum> enums = {GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR, GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA, GL_SRC_ALPHA_SATURATE, GL_SRC1_COLOR, GL_ONE_MINUS_SRC1_COLOR, GL_SRC1_ALPHA, GL_ONE_MINUS_SRC1_ALPHA};
        // static int blendin = 1, blendout = 3;
        // if (ImGui::SliderInt2( "blend", &blendin, 0, enums.size()-1)) glBlendFunc(enums[blendin],enums[blendout]);

        SameLine();

        auto curs = GetCursorPosX();

        SetCursorPosX(GetWindowSize().x-100);

        Text((std::to_string(Member::structs.size())+" struct"+(Member::structs.size()>1?"s":"")).c_str());
        SameLine();
        SetCursorPosX(curs);
        Editor<Log>::cb( node, &logger);

    });
  
    ////////// Effector.HPP

    // Editor<EffectorRef>([](Node* node, EffectorRef *effector){

    //     if (InputInt("wrap", &effector->wrap)) node->update();

    //     for (auto def : effector->definitions) Editor<Effector>::cb( node, def);

    // });

    Editor<Wrappy>([](Node* node, Wrappy *wrap){
        

        for (auto x : wrap->effector_refs) 
            Text(x->effector->name().c_str());

    });
    Editor<Effector>([](Node* node, Effector *def){
        
        auto wrap_ = dynamic_cast<Wrappy*>(def);
        if (wrap_) {
            
            for (auto x : wrap_->effector_refs) ImGui::Text(x->name().c_str());

            return;
            
        }

        static std::map<Effector*,TextEditor> codeeditors;
        auto file_ = dynamic_cast<FileEffector*>(def);
        if (file_) {

            if (codeeditors.find(def) == codeeditors.end()) {

                codeeditors[def].SetShowWhitespaces(false);
                codeeditors[def].SetReadOnly(false);
                codeeditors[def].SetText(file_->file->data.data());
            }

            auto &codeeditor = codeeditors[def];

            codeeditor.Render("codeeditor");

            if (codeeditor.IsTextChanged()) {

                file_->file->data = codeeditor.GetText().c_str();
                file_->load();

                node->update(); // do I need bkp here ? is even the fx useful

            }
            
            return;
            
        }

        draw_definition(def);

    });

    Editor<FileEffector>([](Node* node, FileEffector *e){ Editor<Effector>::cb( node, e); });


    // REMAP.HPP

    // Editor<Universe::Remap>([](Node*node, Universe::Remap* remap){ Editor<Remap>::cb( node, remap); });





    ////////// DRAWCALL.HPP

    Editor<DrawCall>([](Node* node, DrawCall *dc){


        if (ImGui::BeginTabBar("dctqb", ImGuiTabBarFlags_None)) {

            if (ImGui::BeginTabItem("Attribtues")) {

                if (BeginPopupContextItem()) {
                    
                    static std::vector<std::string> BLEND_NAMES = {
                        "GL_ZERO",
                        "GL_ONE",
                        "GL_SRC_COLOR",
                        "GL_ONE_MINUS_SRC_COLOR",
                        "GL_DST_COLOR",
                        "GL_ONE_MINUS_DST_COLOR",
                        "GL_SRC_ALPHA",
                        "GL_ONE_MINUS_SRC_ALPHA",
                        "GL_DST_ALPHA",
                        "GL_ONE_MINUS_DST_ALPHA",
                        "GL_CONSTANT_COLOR",
                        "GL_ONE_MINUS_CONSTANT_COLOR",
                        "GL_CONSTANT_ALPHA",
                        "GL_ONE_MINUS_CONSTANT_ALPHA",
                        "GL_SRC_ALPHA_SATURATE",
                        "GL_SRC1_COLOR",
                        "GL_ONE_MINUS_SRC1_COLOR",
                        "GL_SRC1_ALPHA",
                        "GL_ONE_MINUS_SRC1_ALPHA"

                    };


                    SliderInt(("IN: "+BLEND_NAMES[dc->GL_BLEND_MODE_IN]+"###"+std::to_string(engine.gui_v->member_count++)).c_str(), &dc->GL_BLEND_MODE_IN, 0, BLEND_NAMES.size()-1);
                    SliderInt(("OUT: "+BLEND_NAMES[dc->GL_BLEND_MODE_OUT]+"###"+std::to_string(engine.gui_v->member_count++)).c_str(), &dc->GL_BLEND_MODE_OUT, 0, BLEND_NAMES.size()-1);

                    Text(("instances: " + std::to_string((dc->models.size() == 1 ? dc->models[0]->quantity():1))).c_str());

                    EndPopup();
                }
                for (auto c : node->childrens) {
                    auto effectable = c->is_a_nowarning<Effectable>();
                    if (!effectable) 
                        continue;
                    Editor<Effectable>::cb(c, effectable);
                }
                
                Editor<Effectable>::cb(node, dc);

                ImGui::EndTabItem();

            }

            if (ImGui::BeginTabItem("ShaderProgram")) {

                Editor<ShaderProgram>::cb( node, &dc->shader);

                ImGui::EndTabItem();

            }

            if (ImGui::BeginTabItem("VBO")) {

                Editor<VBO>::cb( node, &dc->vbo);

                ImGui::EndTabItem();

            }
            ImGui::EndTabBar();
        }


    });


    Editor<UberLayer>([](Node* node, UberLayer *ubl){

        // static uint32_t min = 0, max = 10;;
        // for (auto tex : Texture::pool) 
        //     if (ImGui::SliderScalar(("unit"+std::to_string(tex->id)+" "+tex->sampler_name).c_str(), ImGuiDataType_U32, &tex->unit, &min, &max)) 
        //         tex->bind();
            
        // ImVec2 btn_size = {100,50};
        // if (ImGui::Button("smartlayer", btn_size)) {
        //     node->addPtr<UberLayer::VirtualLayer>(&ubl->addLayer(engine.window.width,engine.window.height));
        //     ubl->calc_matrice();
        //     engine.stack->trig(Node::CHANGE);
        //     ubl->update();
        // }
            
        
        Editor<Layer>::cb( node, ubl);

    });

    Editor<Layer>([](Node* node, Layer *layer){
        
        if (ImGui::BeginTabBar("laytab", ImGuiTabBarFlags_None)) {

            if (ImGui::BeginTabItem("main")) {
                            
                if (ImGui::BeginPopupContextItem()){
                        
                    ImGui::Text(("FB "+std::to_string(layer->fb.id)).c_str());

                    ImGui::SliderFloat4("clear_color", &layer->fb.clear_color[0], 0, 1);
                    
                    EndPopup();
                
                }

                Editor<FrameBuffer>::cb( node, &layer->fb);

                ImGui::EndTabItem();

            }

            for (auto x : layer->builder()->samplers) {
                
                if (ImGui::BeginTabItem(x.second->sampler_name.c_str())) {

                    Editor<Texture>::cb( node, x.second);

                    ImGui::EndTabItem();

                }

            }

            ImGui::EndTabBar();

        }


        Editor<DrawCall>::cb( node, layer);

    });


}