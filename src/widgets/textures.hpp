#pragma once
#include "../gui.hpp"
#include "../texture.hpp"

struct TexturesWidget : GUI::Window {

    TexturesWidget() : GUI::Window("Textures") { }

    void draw() override {

        for (int i = 0; i < Texture::pool.size(); i++) {

            std::string wh = std::to_string(Texture::pool[i]->width) + " x " + std::to_string(Texture::pool[i]->height);
            ImGui::Text(wh.c_str());
            
            float ratio = Texture::pool[i]->height/(float)Texture::pool[i]->width;
            auto nw = std::min(Texture::pool[i]->width,(GLuint)512);

            ImGui::Image((void*)(intptr_t)(ImTextureID)(uintptr_t)Texture::pool[i]->id, ImVec2(nw,nw*ratio));

            ImGui::PushID(i+100);

            ImGui::PopID();
            ImGui::Separator();
        }
        
    }

};