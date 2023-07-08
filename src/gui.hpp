#ifndef IMGUI_H
#define IMGUI_H

struct GLFWwindow;

struct GUI {

  GUI(GLFWwindow* window);

  ~GUI();

  void newframe();
  void render();

};

#endif
