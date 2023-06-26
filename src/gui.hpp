#ifndef IMGUI_H
#define IMGUI_H

// link with:
// imgui.cpp imgui_draw.cpp imgui_widgets.cpp imgui_impl_glfw.cpp imgui_impl_opengl3.cpp

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "shader.hpp"



struct GUI {

  struct Element {

    std::string name;

    float def = 0;
    float min = 0;
    float max = 1;

    float* ptr;

    size_t size = 0;

    std::set<ShaderProgram*> links;

    std::vector<float> data;

    void send() {

      for (auto* l:links) {
        
        if (size == 1) l->sendUniform(name.c_str(), *ptr);
        if (size == 2) l->sendUniform(name.c_str(), *ptr, *ptr+1);
        if (size == 3) l->sendUniform(name.c_str(), *ptr, *ptr+1, *ptr+2);
        if (size == 4) l->sendUniform(name.c_str(), *ptr, *ptr+1, *ptr+2, *ptr+3);

      }

    }

    Element(std::string name, size_t size = 1, float def = 0, float min = 0, float max =1, float* ptr = nullptr)
      : name(name), def(def), min(min), max(max), ptr(ptr), size(size) {

      if (ptr) return;

      data.resize(size); 
     
      for (size_t i = 0; i < size; i++) data[i] = def;  // for (auto f:data) f = 1; // marche pas ?!!!

      this->ptr = &data[0];
      
    }

    operator int() { return data[0]; }

    virtual void draw() = 0;
    
  };

  struct Counter : public Element {

    Counter(std::string name) : Element(name) { }

    void draw() override {  data[0]++; }

  };

  struct SliderF : public Element {

    SliderF(std::string name, size_t size = 1, float def = 0, float min = 0, float max = 1, float* ptr = nullptr) 
      : Element(name, size, def, min, max, ptr) { }

    void draw() override { 
      
      if (size == 1) ImGui::SliderFloat(name.c_str(),ptr,min,max); 
      else if (size == 2) ImGui::SliderFloat2(name.c_str(),ptr,min,max); 
      else if (size == 3) ImGui::SliderFloat3(name.c_str(),ptr,min,max); 
      else if (size == 4) ImGui::SliderFloat4(name.c_str(),ptr,min,max); 
      
    }

  };

  struct SliderI : public Element {

    SliderI(std::string name, size_t size = 1, float def = 0, float min = 0, float max = 255, float* ptr = nullptr) 
      : Element(name, size, def, min, max, ptr) { }
      
    void draw() override { 

      std::vector<int> data;

      for (size_t i = 0; i < size; i++) data.push_back(*ptr+i);
      
      if (size == 1) ImGui::SliderInt(name.c_str(),&data[0],min,max); 
      else if (size == 2) ImGui::SliderInt2(name.c_str(),&data[0],min,max); 
      else if (size == 3) ImGui::SliderInt3(name.c_str(),&data[0],min,max); 

      for (size_t i = 0; i < size; i++) ptr[i] = data[i];
      
    }

  };

  std::vector<std::shared_ptr<Element>> elements;

  GUI(GLFWwindow* window);

  ~GUI();

  void newframe();
  void render();
  void draw();
  void draw2();

  Ref<GUI::Element> add(Element* elem, ShaderProgram* shader = nullptr);

  static inline GLint GL_BLEND_MODE_IN = 5;
  static inline GLint GL_BLEND_MODE_OUT = 2;
  static void blendTest();

};

#endif
