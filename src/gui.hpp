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

  void draw() { newframe(); for (auto window : Window::pool) { window->drawFull(); } render(); }

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





#endif
