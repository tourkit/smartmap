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


struct StringsBuffer {

  char* buffer = nullptr;
  const char** pointers = nullptr;

  ~StringsBuffer() { destroy(); }

  void destroy() {

      if (buffer) delete[] buffer;
      if (pointers) delete[] pointers;

  }

  void create(std::vector<const char*> strings) {

        destroy(); 

        size_t names_length = 0; 
        
        for (auto string:strings) names_length += strlen(string) + 1; 

        if (!names_length) names_length+=1;
        buffer = new char[names_length+1];

        memset(buffer,0,names_length+1);

        char* ptr = buffer;

        for (auto string:strings) { strcpy(ptr, string); ptr += strlen(string) + 1; }

        pointers = new const char*[strings.size()];

        for (size_t i = 0; i < strings.size(); i++) { pointers[i] = ptr; ptr += strlen(strings[i]) + 1; }

  }

};




#endif
