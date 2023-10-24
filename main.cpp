/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"

#include "smartmap.hpp"
#include "widgets/artnet_widget.hpp"
// #include "boilerplate.hpp"
// #include "debug.hpp"
// Debug debug;

int main() { 


    // int x = 99;
    // int *y = &x;



    // Boilerplate();

    auto &engine = Engine::getInstance();

    // SmartMap sm;

    // sm.import("config.json");

    // engine.stack.list.push_back(new Stack::DrawCall{&engine.quad, sm.shader, nullptr, nullptr, "Test"});


    // auto *shader = new ShaderProgram({"smartmap.frag", "smartmap.vert"}, true);

    // ArtnetWidget aw(sm.artnet);

    ShaderProgram shader({"smartmap.frag", "smartmap.vert"});

    engine.stack.list.push_back(new Stack::Action{[](){ 

        for (int i = 0; i< File::pool.size(); i++) { 

            File* file = File::pool[i];

            if (file->survey && file->last_modified) {

                auto new_modified = file->getTimeModified();

                if (file->last_modified != new_modified) {
                
                    file->callback(file);

                    std::cout << file->path << " changed " << std::endl;
                    
                }
                

            }

        }

     }, "Files survey"});


    struct FileWidget : GUI::Window {

        FileWidget() : GUI::Window("Files") { 
        
        
        }
        
        void draw() override {

            if (ImGui::BeginTable("table1", 4)) {

                if (false)
                {
                    ImGui::TableSetupColumn("One");
                    ImGui::TableSetupColumn("Two");
                    ImGui::TableSetupColumn("Twos");
                }

                for (int i = 0; i< File::pool.size(); i++) { 

                    File* file = File::pool[i];

                    ImGui::TableNextColumn();
                    ImGui::Checkbox(("##cb"+std::to_string(i)).c_str(), &file->survey);
                    ImGui::TableNextColumn();
                    ImGui::Text(file->name.c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(file->extension.c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(file->last_modified).c_str());

                }
                
                ImGui::EndTable();
            }
        }

    } fw;

    struct StackWidget : GUI::Window {

        // VBO list;
        // Textures list;
        // Framebuffer list;
        // Shader list;

        StringsBuffer vbos,textures, framebuffers, shaders;

        int current = 0;

        Stack *stack;

        StackWidget(Stack *stack) : GUI::Window("Stack"), stack(stack) { 


            std::vector<std::string> names;

            names.resize(0);
            for (auto *vbo : VBO::pool) { names.push_back(vbo->name); }
            vbos.create(names);

            names.resize(0);
            for (auto *tex : Texture::pool) { names.push_back(std::to_string(tex->id)); }
            textures.create(names);

            names.resize(0);
            for (auto *fb : FrameBuffer::pool) { names.push_back(fb->name); }
            framebuffers.create(names);

            names.resize(0);
            for (auto *shader : ShaderProgram::pool) { names.push_back(shader->paths[0]); }
            shaders.create(names);
        
        
        }

        void draw() override {

            if (ImGui::CollapsingHeader("VBOs")) {



                for (auto vbo: VBO::pool) { 

                    // ImGui::Text(std::to_string(vbo->id).c_str());

                };

            }
            for (auto fb: FrameBuffer::pool) { 

                // ImGui::Text(fb->name.c_str() );
            };


            if (ImGui::BeginTable("table1", 5)) {

                if (false)
                {
                    ImGui::TableSetupColumn("One");
                    ImGui::TableSetupColumn("Two");
                    ImGui::TableSetupColumn("Three");
                    ImGui::TableSetupColumn("4");
                    ImGui::TableSetupColumn("5");
                    ImGui::TableHeadersRow();
                }

                int i = 0;

                for (auto cue: stack->list) { 
                    i++;
                    
                    // if (typeid(*cue).name() != typeid(Stack::DrawCall).name()) {
                    // ImGui::TableNextRow(); continue;}


                    ImGui::TableNextColumn();

                    ImGui::Checkbox(("##cb"+std::to_string(i)).c_str(), &cue->active);
                    ImGui::SameLine();
                    ImGui::Text(cue->name.c_str());
                    ImGui::TableNextColumn();
                    if (ImGui::Combo("##wyuaetd1", &current, vbos.buffer)) { ((Stack::DrawCall*)cue)->vbo = VBO::pool[current]; }
                    ImGui::TableNextColumn();
                    ImGui::Combo("##wyuaetd2", &current, textures.buffer);
                    ImGui::TableNextColumn();
                    ImGui::Combo("##wyuaetd3", &current, framebuffers.buffer);
                    ImGui::TableNextColumn();
                    ImGui::Combo("##wyuaetd4", &current, shaders.buffer);

                }
                
                ImGui::EndTable();
            }
        }

    } sw(&engine.stack);

    Engine::Run();
 
}




    

