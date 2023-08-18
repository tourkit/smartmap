#ifndef IMGUI_H
#define IMGUI_H

#include "imgui/imgui.h"
#include "pch.hpp"

struct GLFWwindow;

struct GUI {

  GUI(GLFWwindow* window);

  ~GUI();

  void newframe();
  void render();

  struct Window {

    static inline std::vector<Window*> pool;

    std::string name;
    std::string uid;

    Window(std::string name) : name(name), uid(("window"+std::to_string(pool.size())).c_str()) { pool.push_back(this); }

    void drawFull() {

      ImGui::Begin((name+"##"+uid).c_str());

      draw();

      ImGui::End();

    }

    virtual void draw() {}

  };

};


struct StringsBuffer {

  char* buffer = nullptr;
  const char** pointers = nullptr;

  ~StringsBuffer() { destroy(); }

  void destroy() {

      if (buffer) delete[] buffer;
      if (pointers) delete[] pointers;

  }

  void create(std::vector<const char*> strings) {

        size_t names_length = 0; 
        
        for (auto string:strings) names_length += strlen(string) + 1; 

        if (!names_length) names_length+=1;
        buffer = new char[names_length+1];

        memset(buffer,0,names_length+1);

        char* ptr = buffer;

        for (auto string:strings) { strcpy(ptr, string); ptr += strlen(string) + 1; }

        pointers = new const char*[strings.size()];

        for (size_t i = 0; i < strings.size(); i++) { pointers[i] = ptr; ptr += strlen(strings[i]) + 1; }

        // if (strings.size()) {
        //   if (!strcmp(strings.back(), "6")){ 
            
        //     // std::cout << "yolo " << strings.back() << std::endl;
        //     for (auto s:strings) { std::cout << s << std::endl; }

        //   }
        //   // else {  std::cout << "yala " << strings.back() << std::endl;}
        // }
  }

};

#include "ubo.hpp"

struct UBOWindow : GUI::Window {

  int ubo_current = 3;
  int elem_current = 0;

  StringsBuffer options;

  UBOWindow() : Window("UBOs") {

      std::vector<const char*> names;
      for (auto ubo:UBO::pool) { names.push_back(ubo->name); }
      options.create(names);

  }

  void draw() override {

        ImGui::Combo("Select UBO", &ubo_current, options.buffer);
        
        // ImGui::Text(("Conponents : "+std::to_string(UBO::pool[ubo_current]->definition.components.size())
        //   +" | Members : "+std::to_string(UBO::pool[ubo_current]->definition.members.size())
        //   +" | Quantity : "+std::to_string(UBO::pool[ubo_current]->definition.quantity)).c_str());

        int uniform_offset = 0;
        ImGui::SliderInt("current##uibocurrent", &elem_current,0,UBO::pool[ubo_current]->definition.quantity);
        for (auto c:UBO::pool[ubo_current]->definition.components) {
            
            if (ImGui::CollapsingHeader(c->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            
                for (auto m:c->members) {

                  if (ImGui::SliderFloat((m.name+"##"+c->name+m.name+uid).c_str(), &UBO::pool[ubo_current]->data[uniform_offset+++(elem_current*UBO::pool[ubo_current]->definition.members.size())], m.range_from, m.range_to)) { 
                      
                      UBO::pool[ubo_current]->update(); 
                  }
                
                }
                
            }else { uniform_offset += c->members.size(); }
                 
        }


  }

};



#endif
