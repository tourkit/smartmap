#include "editor.hpp"

#include "log.hpp"
#include "node.hpp"
#include "gui.hpp"
#include "buffer.hpp"
#include "folder.hpp"
#include "struct.hpp"
#include "output.hpp"
#include "ndi.hpp"
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
#include <format>
static int hovered_offset = -1;
static int hovered_size = -1;
static bool is_hovered = false;
static uint32_t is_hovered_winID = 0;

namespace ImGui {

    static void TextX(std::string label, int offset, int size, int depth, std::vector<float> range = {}) {

        for (int i = 0; i < depth; i++) label = "    "+label;

        if (range.size()==2) label += " ( " +std::format("{}",range[0])+", "+std::format("{}", range[1])+" )";

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

static void draw_definition(Member *member, int offset = 0, int depth = 0) {

    if (member->isRef()) {
        member = member->members[0];
        }
    std::vector<float> range;

    if (member->isData()) {

        range.resize(2);

        float from = *(float*)member->range_from_ptr ;
        float to = *(float*)member->range_to_ptr ;
        size_t size = sizeof(float);

        if (member->type() == typeid(int)) {

            from = *(int*)member->range_from_ptr;
            to = *(int*)member->range_to_ptr;
            size = sizeof(int);

        }
        if (member->type() == typeid(char)) {

            from = *(char*)member->range_from_ptr;
            to = *(char*)member->range_to_ptr;
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

        ImGui::BeginDisabled();

         if (size > 1025) size = 1025;
    for (int member_count = 0; member_count < size; member_count++) {


        ImGui::PushID(member_count);
        if (is_hovered_winID == ImGui::GetWindowDockID() && is_hovered && member_count == hovered_offset) {

            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(.5,0,0,1));

            colorized = true;

        }

        if (member_count && !(member_count%cells_per_line)) ImGui::NewLine();
        ImGui::SameLine(((member_count%cells_per_line)*(cell_width+cell_margin)));

        ImGuiDataType_ datatype = ImGuiDataType_U8;
        ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, ((*(((uint8_t*)data)+member_count))/255.0f)*26);
        if (ImGui::VSliderScalar("",  ImVec2(cell_width,30),    datatype, &ndata,  &cell_min,   &cell_max,   "")) {


        }

        ImGui::PopStyleVar(1);

        ImGui::SameLine(0);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - cell_width) ;
        std::string str = std::to_string(member_count);
        ImGui::Text(str.c_str());

        ImGui::PopID();
        if (colorized && (member_count == hovered_offset+hovered_size-1 || member_count == size-1)) { ImGui::PopStyleColor(); colorized= false; }

    }

    ImGui::EndDisabled();

    ImGui::SetWindowFontScale(1);

    ImGui::PopStyleVar(5);

}

static bool draw_guis(Buffer* buff, Member* member = nullptr, uint32_t offset = 0) {




    static int member_count = 0;

    if (!member) {

        member = buff;
         member_count = 0;

        }
    else if (member->isRef()) member = member->members[0];

    struct int_ { int val = 0; };
    static std::map<Member*,int_> elem_currents;
    int &elem_current = elem_currents[member].val;

    if (member->quantity() > 1 ) {

        if (ImGui::SliderInt(("instance##current"+member->name()).c_str(), &elem_current, 0, member->quantity()-1)) { }

        offset += member->footprint()*elem_current;

    }

    bool has_changed = false;



    for (auto& m : member->members) {

        if (m->isData()) {

                static int t_range_i = 65535;
                static float t_range_f = 1.0f;
                static int f_range_i = 0;
                static float f_range_f = 0;

                void* range_from;
                if (m->range_from_ptr) range_from = m->range_from_ptr;
                else range_from = &f_range_f;
                range_from = &f_range_f;

                void* range_to;
                if (m->range_to_ptr) range_to = m->range_to_ptr;
                else range_to = &t_range_f;
                range_to = &t_range_f;

                if (m->range_from_ptr) range_from = m->range_from_ptr;
                if (m->range_to_ptr) range_to = m->range_to_ptr;


                auto type = ImGuiDataType_Float;

                if (m->type() == typeid(int)) {

                    type = ImGuiDataType_U16;

                    if (m->range_from_ptr) f_range_i = *(int*)m->range_from_ptr;
                    if (m->range_to_ptr) t_range_i = *(int*)m->range_to_ptr;

                    range_to = &t_range_i;
                    range_from = &f_range_i;

                }

                int q = 1;
                if (m->type() == typeid(glm::vec2)) q = 2;
                if (m->type() == typeid(glm::vec3)) q = 3;
                if (m->type() == typeid(glm::vec4)) q = 4;

                std::string name = (m->name()+"##SIE"+member->name()+m->name()+std::to_string(member_count++));

                if (ImGui::SliderScalarN(name.c_str(), type, buff->data.data()+offset, q, range_from, range_to)) has_changed = true;

        }else{

            std::string septxt  = m->name();

            septxt+= "(" + std::to_string(m->quantity()) + ")";

            ImGui::SeparatorText(septxt.c_str());

            if (draw_guis(buff, m, offset)) has_changed = true;

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


void Editors::init() {

    // ////////// xxx.HPP

    // Editor<xxx>([](Node* node, xxx *x){ });

    ////////// Artnet.HPP



    Editor<NDI::Sender>([](Node*node, NDI::Sender* sender){



        Editor<Output>::cb(node, sender);


            ImGui::NewLine();
            ImGui::NewLine();

        if (sender->pixelcount) draw_raw( &sender->data[0], sender->pixelcount );


    });

    Editor<DMXRemap>([](Node*node, DMXRemap* remap){

        ImGui::SetNextItemWidth(500);
        if (ImGui::BeginTable("##remapswindow", 7, ImGuiTableFlags_Borders, ImVec2(550, 0))) {

            ImGui::TableSetupColumn("name");
            ImGui::TableSetupColumn("skip", ImGuiTableColumnFlags_WidthFixed, 50);
            ImGui::TableSetupColumn("coarse", ImGuiTableColumnFlags_WidthFixed, 50);
            ImGui::TableSetupColumn("fine", ImGuiTableColumnFlags_WidthFixed, 50);
            ImGui::TableSetupColumn("ultra", ImGuiTableColumnFlags_WidthFixed, 50);
            ImGui::TableSetupColumn("min/max", ImGuiTableColumnFlags_WidthFixed, 200);
            ImGui::TableSetupColumn("active", ImGuiTableColumnFlags_WidthFixed, 50);
            ImGui::TableHeadersRow();

            int member_id = 0;

            remap->dst->def()->each([&](auto& inst) {

                auto *m = inst.def();

                 if (m->isData()) {

                    ImGui::TableNextRow();

                    // if (ImGui::IsItemHovered()) ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(.5,.5,.5,1));

                    ImGui::TableNextColumn();
                    // auto textWidth   = 100-ImGui::CalcTextSize(m->name().c_str()).x;
                    // ImGui::SetCursorPosX((textWidth * 0.5f) );
                    ImGui::Text((m->name()).c_str());

                    if (member_id < remap->attributes.size()) {

                        static int e = 0;

                        ImGui::TableNextColumn();
                        ImGui::RadioButton(("##arbt"+std::to_string(member_id)).c_str(), &remap->attributes[member_id].combining , 0);
                        ImGui::TableNextColumn();
                        ImGui::RadioButton(("##vrbt"+std::to_string(member_id)).c_str(), &remap->attributes[member_id].combining , 1);
                        ImGui::TableNextColumn();
                        ImGui::RadioButton(("##xrbt"+std::to_string(member_id)).c_str(), &remap->attributes[member_id].combining , 2);
                        ImGui::TableNextColumn();
                        ImGui::RadioButton(("##srbt"+std::to_string(member_id)).c_str(), &remap->attributes[member_id].combining , 3);
                        ImGui::TableNextColumn();
                        ImGui::SetNextItemWidth(200);
                        ImGui::InputFloat2(("##minmax"+std::to_string(member_id)).c_str(), &remap->attributes[member_id].min);
                        ImGui::TableNextColumn();
                        ImGui::Checkbox(("##arsbt"+std::to_string(member_id)).c_str(), &remap->attributes[member_id].active);

                    }

                    member_id+=m->count();

                    // if (ImGui::IsItemHovered()) ImGui::PopStyleColor();


                }

            });

                ImGui::EndTable();
            }

        std::stringstream deststart;
        deststart  <<  (void*)remap->dst->data();
        deststart  << " - " << (void*)remap->dst->buff->data.data();
        deststart  << " - " << remap->dst->offset;
        ImGui::Text(deststart.str().c_str()) ;

        std::stringstream buffa;
        buffa << remap->src->def()->name() << " - " << remap->src;
        std::stringstream buffb;
        buffb << remap->dst->def()->name() << " - " << remap->dst;

        ImGui::NewLine(); ImGui::SetNextItemWidth( ImGui::CalcTextSize( (char*)buffa.str().c_str() ).x); ImGui::InputText("##puppyaaa", (char*)buffa.str().c_str(), 10, ImGuiInputTextFlags_ReadOnly);

        ImGui::SameLine(); ImGui::Text(" -> ");

        ImGui::SameLine(); ImGui::SetNextItemWidth( ImGui::CalcTextSize( (char*)buffb.str().c_str() ).x); ImGui::InputText("##puppybbb", (char*)buffb.str().c_str(), 10, ImGuiInputTextFlags_ReadOnly);

        ImGui::SameLine(); ImGui::Text(("* "+std::to_string(remap->dst->def()->quantity())).c_str());

        // engine.
    });

    Editor<Artnet>([](Node*node,Artnet* an){

        ImGui::Text(an->name().c_str());

        ImGuiTextBuffer tbuff;
        for (auto x : engine.available_ips) tbuff.append(x.c_str(), x.c_str()+x.length()+1);

        if (Combo("device##available_ips", &an->device_id, tbuff.begin())) {

            node->active(false);
            an->connect(engine.available_ips[an->device_id]);

        }

        for (auto x : an->universes) {
            std::string str;

            if (!x.second.get()->instances.size()) continue;

            auto &inst = x.second.get()->instances[0];

            str = "universe "+std::to_string(x.first) + " " + std::to_string(inst.offset) + " " + x.second.get()->name();
            ImGui::Text(str.c_str());

            ImGui::NewLine();
            draw_raw(x.second.get()->instances[0].data(), 512);
            ImGui::NewLine();

        };

        // ImGui::Text( std::to_string(an->footprint_all()).c_str() );
        // Editor<Buffer>::cb(node, an);

    });

    ////////// STRUCT.HPP

    // TOFIX

    // Editor<Struct>([](Node* node, Struct *s){

    //     std::string str = s->name+" " +std::to_string(s->size());
    //     ImGui::Text(str.c_str());

    //     for (auto& c : s->comps) {

    //         std::string str = " - "+c->name+" "+std::to_string(c->size);
    //         ImGui::Text(str.c_str());

    //         for (auto& m : c->members) {
    //             std::string str = "  -- "+m.name+" "+std::to_string(m.size);
    //             ImGui::Text(str.c_str());
    //         }


    //     }

    // });

    ////////// WINDOW.HPP

    Editor<Window>([](Node* node, Window *window){

        using namespace ImGui;

        Editor<Output>::cb(node, window);


        NewLine();

        int i = 0;

        for (auto &display : engine.window.displays) {

            if (Button( ("fit display "+std::to_string(i++)).c_str())) {

                window->pos( display.x, display.y );
                window->size( display.width, display.height );

            }
            SameLine();
        }
        NewLine();
        NewLine();

    });

    ////////// OUTPUT.HPP

    Editor<Output>([](Node* node, Output *output){

        int x = 0;

        ImGui::Combo("source", &x, "layer1");

        static uint32_t min = 0;

        if (ImGui::DragScalarN("position###winpos", ImGuiDataType_U32,  &output->offset_x, 2, 1, &min)) {

            output->pos( output->offset_x, output->offset_y );

        }

        if (ImGui::DragScalarN("size###winsize", ImGuiDataType_U32,  &output->width, 2, 1, &min)) {

            output->size( output->width, output->height );

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

                    auto oldPos = frageditor.GetCursorPosition();
                    frageditor.SetText(shader->frag.src);
	                frageditor.SetCursorPosition(oldPos);

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
        static ImVec4 warning = ImVec4(1.0f, 0.7f, 0.0f, 1.0f);
        static ImVec4 error = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
        static ImVec4 verbose = ImVec4(0.4f, 0.4f, 1.0f, 1.0f);

        static float *curr = &info.x;

        ImGui::SameLine();if(ImGui::ColorButton("info", info)){ curr = &info.x; ImGui::OpenPopup("picker");  }
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
            std::string str = " "+m.msg;
            ImGui::Text(str.c_str());
            ImGui::PopStyleColor();


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
        memcpy(path,file->path.c_str(),file->path.size());

        if (ImGui::InputText("path", path, 512)) {

            if (strcmp(path, file->path.c_str())) {

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

            node->bkpupdate();

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

    // tofix

        ImGui::Separator();

        draw_definition(buffer);

        ImGui::Separator();

        draw_raw(buffer->data.data(),buffer->data.size());

        ImGui::Separator();

        // draw_guis(buffer->data.data(),buffer->data.size());

        // static StringsBuffer object_str;
        // static int obj_current = 0;
        // std::vector<std::string> obect_strs;
        // for (auto &m : buffer->members) obect_strs.push_back(m->name());
        // if (!obect_strs.size()) return;
        // object_str.create(obect_strs);
        // ImGui::Combo("Buffer##234sdfgsdfg", &obj_current, object_str.buffer);

        // auto inst = (*buffer)[obj_current];

        // if (obj_current <= buffer->members.size()-1)
        if (draw_guis(buffer)) node->update();

    });

    // tofix

    // Editor<Layer>([](Node* node, Layer *layer){

    //     Editor<Texture>::cb(node, layer->fb.texture);

    //     Editor<ShaderProgram>::cb(node, &layer->shader);

    //     ImGui::Separator();
    //     Editor<Object>::cb(node, layer->vbo.vertices);
    //     ImGui::Separator();
    //     Editor<Object>::cb(node, layer->vbo.indices);
    //     ImGui::Separator();
    //     Editor<VBO>::cb(node, &layer->vbo);

    // });

    ////////// UBO.HPP

    Editor<UBO>([](Node* node, UBO *ubo){

        std::string subs_str = "data[" + std::to_string(ubo->data.size())+"]";
        // std::string subs_str = std::to_string(ubo->subscribers.size())+" sub(s):";
        // for (auto s: ubo->subscribers) subs_str += " #"+std::to_string(s->id);
        ImGui::Text(subs_str.c_str());

        Editor<Buffer>::cb(node, ubo);

    });

    ////////// MODEL.HPP

    Editor<Model>([](Node* node, Model *model){


        ImGui::Text(("effectorz " + std::to_string(model->effectors.size())).c_str());

        if (ImGui::InputInt("quantity##qqqqlalal" , &model->s.quantity_v)) { model->s.quantity(model->s.quantity_v); node->update(); }

        if (draw_guis(&engine.dynamic_ubo))engine.dynamic_ubo.upload();

    });

    ////////// Artnet.HPP

    // Editor<Universe>([](Node*node,Universe* dmx){



    //     for (auto &r : dmx->remaps) Editor<Universe::Remap>::cb(node, &r);

    // });

    ////////// VBO.HPP

    Editor<VBO>([](Node*node,VBO*vbo){ Editor<Buffer>::cb(node, vbo); });

    ////////// Effector.HPP

    Editor<Effector>([](Node* node, Effector *effector){

        if (effector->file) Editor<File>::cb(node, effector->file);

        for (auto x : Effector::get(effector->file).args)  {

            std::string out_str = x.first+ " "+x.second;

            if ( Effector::get(effector->file).ranges.find(x.second) !=  Effector::get(effector->file).ranges.end()) for (auto x :  Effector::get(effector->file).ranges[x.second])  out_str += " " + std::to_string(x).substr(0,3) + ",";

            ImGui::Text(out_str.c_str());
        }

    });


    // REMAP.HPP

    // Editor<Universe::Remap>([](Node*node, Universe::Remap* remap){ Editor<Remap>::cb(node, remap); });

    ////////// Engine.HPP

    Editor<Stack>([](Node* node, Stack *stack){ Editor<Log>::cb(node, &logger); });

    Editor<Debug>([](Node* node, Debug *debug){ Editor<Log>::cb(node, &logger); });

    ////////// DRAWCALL.HPP

    Editor<DrawCall>([](Node* node, DrawCall *dc){ Editor<ShaderProgram>::cb(node, &dc->shader); Editor<VBO>::cb(node, &dc->vbo); });

    Editor<Layer>([](Node* node, Layer *layer){

        if (ImGui::BeginTabBar("laytab", ImGuiTabBarFlags_None)) {

            if (ImGui::BeginTabItem("main")) {

                Editor<Texture>::cb(node, layer->fb.texture);

                ImGui::EndTabItem();

            }
            if (ImGui::BeginTabItem("1st pass")) {

                Editor<Texture>::cb(node, layer->feedback);

                ImGui::EndTabItem();

            }


            ImGui::EndTabBar();

        }


    Editor<DrawCall>::cb(node, layer);  });

    ////////// Atlas.HPP

    Editor<Atlas>([](Node* node, Atlas *atlas){

        Editor<Texture>::cb(node, atlas->texture);

        // Editor<Object>::cb(node, atlas->buffer);  // tofix


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

}



/////// EditorWidget.hpp

EditorWidget::EditorWidget() : GUI::Window("Editor")  {  }

EditorWidget::~EditorWidget()  { std::erase_if(engine.gui->editors, [this](EditorWidget* e) { return e == this; }); }


void EditorWidget::draw() {

    if (!selected || !locked) selected = engine.selected;

    std::string name = selected->name();
    // this->name = name;

    if (selected->parent()) { name = selected->parent()->name() + "::" + name;  }

    ImGui::PushStyleColor(ImGuiCol_Text, selected->color);
    ImGui::Text("%s", name.c_str());
    ImGui::PopStyleColor(1);

    if (ImGui::IsItemClicked()) ImGui::OpenPopup("nodecolorpicker");
    if (ImGui::BeginPopup("nodecolorpicker")) { ImGui::ColorPicker4("#nodecolorpickercolor", &selected->color.x); ImGui::EndPopup(); }

    ImGui::SameLine(); ImGui::Checkbox(("lock##locked"+std::to_string((size_t)this)).c_str(), &locked);

    std::string referings;
    for (auto r : selected->referings) if (r) referings += (r->name())+", ";
    if (referings.length()) {
        std::string str = "("+referings.substr(0,referings.length()-2)+")";
        ImGui::SameLine(); ImGui::Text(str.c_str()); }

    selected->editor();

}
