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

    SmartMap sm;

    sm.import("config.json");

    // engine.stack.list.push_back(new Stack::DrawCall{&engine.quad, sm.shader, nullptr, nullptr, "DC main"});


    // auto *shader = new ShaderProgram({"smartmap.frag", "smartmap.vert"}, true);

    // ArtnetWidget aw(sm.artnet);


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

            stack->list.push_back(new Stack::Action([](){}));
        
        
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

            for (auto cue: stack->list) { 

                // if (typeid(*cue).name() != typeid(Stack::DrawCall).name()) return;
                // ImGui::Text(cue->name.c_str());

            // for (auto cue: engine.stack.list) 
            // std::cout <<typeid(*cue).name() << std::endl;
                ImGui::Text(cue->name.c_str());
                ImGui::SameLine();
                ImGui::SetNextItemWidth(100);
                ImGui::Combo("##wyuaetd1", &current, vbos.buffer);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(100);
                ImGui::Combo("##wyuaetd2", &current, textures.buffer);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(100);
                ImGui::Combo("##wyuaetd3", &current, framebuffers.buffer);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(100);
                ImGui::Combo("##wyuaetd4", &current, shaders.buffer);

            }

        }

    } sw(&engine.stack);

    Engine::Run();
 
}




    

