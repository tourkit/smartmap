#ifndef IMGUI_H
#define IMGUI_H

// link with:
// imgui.cpp imgui_draw.cpp imgui_widgets.cpp imgui_impl_glfw.cpp imgui_impl_opengl3.cpp

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "globals.hpp"



struct GUI {

  struct Element {

    std::string name;

    float def = 0;
    float min = 0;
    float max = 1;

    std::set<GL::ShaderProgram*> links;

    std::vector<float> data;

    void send() {

      for (auto* l:links) {
        
        if (data.size() == 1) l->sendUniform(name.c_str(), data[0]);
        if (data.size() == 2) l->sendUniform(name.c_str(), data[0],data[1]);
        if (data.size() == 3) l->sendUniform(name.c_str(), data[0],data[1],data[2]);
      }

    }

    Element(std::string name, size_t size = 1, float def = 0, float min = 0, float max =1)
      : name(name), def(def), min(min), max(max)  { init(size); }

    void init(size_t size) {

      data.resize(size); 
     
      for (size_t i = 0; i < data.size(); i++) data[i] = def;  // for (auto f:data) f = 1; // marche pas ?!!!

    }

    virtual void draw() = 0;
    
  };

  struct Counter : public Element {

    Counter(std::string name) : Element(name) { }

    void draw() override {  data[0]++; }

  };

  struct SliderF : public Element {

    SliderF(std::string name, size_t size = 1, float def = 0, float min = 0, float max =1) 
      : Element(name, size, def, min, max) { }

    void draw() override { 
      
      if (data.size() == 1) ImGui::SliderFloat(name.c_str(),&data[0],min,max); 
      else if (data.size() == 2) ImGui::SliderFloat2(name.c_str(),&data[0],min,max); 
      else if (data.size() == 3) ImGui::SliderFloat3(name.c_str(),&data[0],min,max); 
      
    }

  };

  struct SliderI : public Element {

    SliderI(std::string name, size_t size = 1, float def = 0, float min = 0, float max = 255) 
      : Element(name, size, def, min, max) { }
      
    void draw() override { 

      std::vector<int> data;
      data.resize(this->data.size());
      
      if (data.size() == 1) ImGui::SliderInt(name.c_str(),&data[0],min,max); 
      else if (data.size() == 2) ImGui::SliderInt2(name.c_str(),&data[0],min,max); 
      else if (data.size() == 3) ImGui::SliderInt3(name.c_str(),&data[0],min,max); 

      for (size_t i = 0; i < data.size(); i++) this->data[i] = data[i];
      
    }

  };

  std::set<std::shared_ptr<Element>> elements;

  GUI(GLFWwindow* window);

  ~GUI();

  void newframe();
  void render();
  void draw();

};

#endif
