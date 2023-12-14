#ifndef IMGUI_H
#define IMGUI_H

#include "imgui/imgui.h"
#include "pch.hpp"

struct GLFWwindow;

struct GUI {
  
  bool draw_gui = false;

  GUI(GLFWwindow* window);

  ~GUI();

  void newframe();
  void render();

  void draw() { 
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    newframe(); 
    if (draw_gui) {

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

      ImGui::Begin((name+"##"+uid).c_str());

      draw();

      ImGui::End();

    }

    virtual void draw() {}

  };

};





#endif
