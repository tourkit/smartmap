#include "../gui.hpp"
#include "../stack.hpp"

struct StackWidget : GUI::Window {

        // VBO list;
        // Textures list;
        // Framebuffer list;
        // Shader list;

        StringsBuffer vbos,textures, framebuffers, shaders;

        int current = 0;
        // int current_vbo, current_tex = 0;
        std::vector<std::array<int,5>> currents;

        Stack *stack;

        StackWidget(Stack *stack) : GUI::Window("Stack"), stack(stack) { 

            update();

        }

        void update() {

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

            currents.resize(stack->list.size());

            for (int i = 0; i< stack->list.size(); i++) { 

                Stack::Cue* cue = stack->list[i];         

                if (typeid(*cue).hash_code() == typeid(Stack::DrawCall).hash_code()) {

                    auto *dc = (Stack::DrawCall*)cue;

                    currents[i][0] = std::distance(VBO::pool.begin(), std::find(VBO::pool.begin(), VBO::pool.end(), dc->vbo));
                    currents[i][1] = std::distance(Texture::pool.begin(), std::find(Texture::pool.begin(), Texture::pool.end(), dc->texture));
                    currents[i][2] = std::distance(FrameBuffer::pool.begin(), std::find(FrameBuffer::pool.begin(), FrameBuffer::pool.end(), dc->fb));
                    currents[i][3] = std::distance(ShaderProgram::pool.begin(), std::find(ShaderProgram::pool.begin(), ShaderProgram::pool.end(), dc->shader));

                }

            }
        
        }

        void draw() override {

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


                // int i = 0;
                // for (auto cue: stack->list) { 
                //     i++;
                for (int i = 0; i< stack->list.size(); i++) { 

                    Stack::Cue* cue = stack->list[i];
                    
                    // if (typeid(*cue).name() != typeid(Stack::DrawCall).name()) {
                    // ImGui::TableNextRow(); continue;}

                    ImGui::TableNextColumn();

                    ImGui::Checkbox(("##xxcb"+std::to_string(i)).c_str(), &cue->active);
                    ImGui::SameLine();
                    ImGui::Text(cue->name.c_str());
                    ImGui::TableNextColumn();
                    if (ImGui::Combo(("##xxvbo"+std::to_string(i)).c_str(), &currents[i][0], vbos.buffer)) { ((Stack::DrawCall*)cue)->vbo = VBO::pool[currents[i][0]]; }
                    ImGui::TableNextColumn();
                    if (ImGui::Combo(("##xxtex"+std::to_string(i)).c_str(), &currents[i][1], textures.buffer)) { ((Stack::DrawCall*)cue)->texture = Texture::pool[currents[i][1]]; }
                    ImGui::TableNextColumn();
                    if (ImGui::Combo(("##xxfb"+std::to_string(i)).c_str(), &currents[i][2], framebuffers.buffer)) { ((Stack::DrawCall*)cue)->fb = FrameBuffer::pool[currents[i][2]]; }
                    ImGui::TableNextColumn();
                    if (ImGui::Combo(("##xxshader"+std::to_string(i)).c_str(), &currents[i][3], shaders.buffer)) { ((Stack::DrawCall*)cue)->shader = ShaderProgram::pool[currents[i][3]]; }

                }
                
                ImGui::EndTable();
            }
        }

    };