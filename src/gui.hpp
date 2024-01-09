#ifndef IMGUI_H
#define IMGUI_H

#include "imgui/imgui.h"
#include "pch.hpp"

struct GLFWwindow;

struct GUI {

  void *dragging = nullptr;
  
  bool draw_gui = false;

  GUI(GLFWwindow* window);

  ~GUI();

  void newframe();
  void render();

  void draw() { 
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    newframe(); 
    if (draw_gui) {
      ImGui::ShowDemoWindow();
      for (auto window : Window::pool) { window->drawFull(); } 
      

    } 

    render(); 
  }

  struct Window {

    static inline std::vector<Window*> pool;

    std::string name;
    std::string uid;
    bool active = true;

    Window(std::string name) : name(name), uid(("window"+std::to_string(pool.size())).c_str()) { pool.push_back(this); }

    void drawFull() {

      if (!active) return;

      bool p_open = true;
      ImGui::Begin((name+"##"+uid).c_str(), &p_open, ImGuiWindowFlags_MenuBar);

      draw();

      ImGui::End();

    }

    virtual void draw() {}
    virtual void drawTree() { draw(); }
    virtual void drawEditor() {}

  };

};





#endif
