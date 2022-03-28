#ifndef SHADERTOY_H
#define SHADERTOY_H

#include "gui.hpp"
#include "TreeviewWindow.h"

#include <fstream>
#include <iostream>

#include "vendor/ImGuiColorTextEdit/TextEditor.h"
#include "src/renderer.hpp"

#pragma once


struct Shadertoy : Renderer, GUI::Panel {

    TextEditor editor[2];

    std::vector<float> uniforms{1,2,3};
    
    ImGuiIO& io = ImGui::GetIO();
    ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\projects\\cpp\\smartmap\\smartmap\\include\\vendor\\imgui\\misc\\fonts\\ProggyClean.ttf", 18);

    Shadertoy(const char* label) : Renderer("ShaderToy") {

      std::cout << "new" << std::endl;

      editor[0] = TextEditor((std::string("assets/shader/")+label+".vert").c_str());
      editor[1] = TextEditor((std::string("assets/shader/")+label+".frag").c_str());

    }

    void draw() override {
      
      	ImGui::Begin("ShaderToy", NULL, ImGuiWindowFlags_NoCollapse);
        
        for (float& uniform : uniforms) { ImGui::SliderFloat("float", &uniform, 0.0f, 1.0f);}
        
        ImGui::Separator();

        ImGui::PushFont(font);

		    editor[0].Render("shdrty1" , ImVec2(0,200) );
        
        ImGui::Separator();
		    
        editor[1].Render("shdrty2");
        
        ImGui::PopFont();

        ImGui::End();

    }

    // ~Shadertoy();


};

#endif