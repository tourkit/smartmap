#ifndef SHADERTOYWINDOW_H
#define SHADERTOYWINDOW_H

#include "gui.hpp"
#include "TreeviewWindow.h"

#include <fstream>

#include "vendor/ImGuiColorTextEdit/TextEditor.h"

#pragma once

struct ShadertoyWindow : GUI::Renderer , Node {

    TextEditor editor[2];

    std::vector<float> uniforms{1,2,3};
    
    
    ImGuiIO& io = ImGui::GetIO();
    ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\projects\\cpp\\smartmap\\smartmap\\include\\vendor\\imgui\\misc\\fonts\\ProggyClean.ttf", 18);



    ShadertoyWindow(const char* label) {

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

    // ~ShadertoyWindow();


};

#endif