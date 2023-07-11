#ifndef IMGUI_H
#define IMGUI_H

#include "imgui/imgui.h"

struct GLFWwindow;

struct GUI {

  GUI(GLFWwindow* window);

  ~GUI();

  void newframe();
  void render();

};

#endif
