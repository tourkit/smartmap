#ifndef IMGUI_H
#define IMGUI_H

// link with:
// imgui.cpp imgui_draw.cpp imgui_widgets.cpp imgui_impl_glfw.cpp imgui_impl_opengl3.cpp

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_glfw.h"

#include "shader.hpp"



struct GUI {

  GUI(GLFWwindow* window);

  ~GUI();

  void newframe();
  void render();

  static inline GLint GL_BLEND_MODE_IN = 5;
  static inline GLint GL_BLEND_MODE_OUT = 2;
  static void blendTest();

};

#endif
