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

#include "ubo.hpp"

struct UBOWindow : GUI::Window {

  int ubo_current = 3;
  int elem_current = 0;
  char* buffer;
  const char** pointers;

  ~UBOWindow() { 

      delete[] buffer;
      delete[] pointers;

  }
  UBOWindow() : Window("UBOs") {

        size_t names_length = 0; 
        for (auto ubo:UBO::pool) names_length += strlen(ubo->name) + 1; 
        buffer = new char[names_length+1]; 
        char* ptr = buffer;
        for (auto ubo:UBO::pool) { strcpy(ptr, ubo->name); ptr += strlen(ubo->name) + 1; }
        pointers = new const char*[UBO::pool.size()];
        for (size_t i = 0; i < UBO::pool.size(); i++) { pointers[i] = ptr; ptr += strlen(UBO::pool[i]->name) + 1; }

  }

  void draw() override {

        ImGui::Combo("Select UBO", &ubo_current, buffer);
        
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
