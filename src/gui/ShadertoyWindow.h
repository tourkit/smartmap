#ifndef SHADERTOY_H
#define SHADERTOY_H

#include "gui.hpp"
#include "TreeviewWindow.h"

#include <fstream>
#include <iostream>

#include "vendor/ImGuiColorTextEdit/TextEditor.h"
#include "src/renderer.hpp"
#include "src/utils/string.h"

#pragma once

struct Attribute {
  
  enum Type{

    NUMBER,
    STRING,
    IMAGE,
    UNKNOW,

  } type;

  Attribute(std::string name, Type type) : name(name), type(type) {}

  std::string name;

  virtual void gui() = 0;
  
};

struct Number : Attribute {

  float value;

    void gui() override {    
  
      ImGui::SliderFloat(name.c_str(), &value, 0.0f, 1.0f);
  
    }

    Number(std::string name = "number", float value = 0.0f) : Attribute(name, NUMBER), value(value) { }

};

struct Shadertoy : Renderer, GUI::Panel {

    TextEditor editor[2];

    std::vector<Attribute*> uniforms;
    
    ImGuiIO& io = ImGui::GetIO();
    ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\projects\\cpp\\smartmap\\smartmap\\include\\vendor\\imgui\\misc\\fonts\\ProggyClean.ttf", 18);

    Shadertoy(const char* label) : Renderer("ShaderToy") {

      editor[0] = TextEditor((std::string("assets/shader/")+label+".vert").c_str());
      editor[1] = TextEditor((std::string("assets/shader/")+label+".frag").c_str());

      update();

    }

    void update() {

    auto list = String::find(editor[1].GetText(), "uniform\\s([^\\s]+)\\s([\\w\\d_]+)");

    for (auto& uniform : list) {

        auto splited = String::split(uniform, ' ');
        uniforms.push_back(new Number(splited[2]));

      }

    }

    void draw() override {
      
      	ImGui::Begin("ShaderToy", NULL, ImGuiWindowFlags_NoCollapse);

        bool alternateBG = 0;
        
        for (auto& uniform : uniforms) { 
          // ImGui::PushStyleVar

          if (alternateBG) { alternateBG = 0; } else { alternateBG = 1; }
          
          uniform->gui(); 

          ImGui::Separator();
          
        }
        
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