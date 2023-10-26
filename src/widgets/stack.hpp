#include "../gui.hpp"
#include "../stack.hpp"

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
            for (auto *shader : ShaderProgram::pool) { names.push_back(shader->shaders[0].get()->file.name); }
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

    };