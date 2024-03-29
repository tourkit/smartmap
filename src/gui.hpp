#pragma once


#include <vector>
#include <string>
#include <cstring>

#include "imgui/imgui.h"
#include "vendors/ImGuiColorTextEdit/TextEditor.h"

struct EditorWidget;
struct TreeWidget;

struct StringsBuffer {

  char* buffer = nullptr;
  const char** pointers = nullptr;

  ~StringsBuffer() { destroy(); }

  void destroy() {

      if (buffer) delete[] buffer;
      if (pointers) delete[] pointers;

  }

  void create(std::vector<std::string> strings) {

        destroy(); 

        size_t names_length = 0; 
        
        for (auto string:strings) names_length += string.size() + 1; 

        if (!names_length) names_length+=1;
        buffer = new char[names_length+1];

        memset(buffer,0,names_length+1);

        char* ptr = buffer;

        for (auto string:strings) { strcpy(ptr, string.c_str()); ptr += string.size() + 1; }

        pointers = new const char*[strings.size()];

        for (size_t i = 0; i < strings.size(); i++) { pointers[i] = ptr; ptr += strings[i].size() + 1; }

  }

  StringsBuffer() {}
  
  StringsBuffer(std::vector<std::string> strings) { create(strings); }

};

struct GLFWwindow;

struct GUI {

  void *dragging = nullptr;
  
  bool draw_gui = true;

  bool demo = false;
  
  GUI(GLFWwindow* window);

  ~GUI();

  void newframe();
  void render();

  void draw() ;

  std::vector<EditorWidget*> editors;
  std::vector<TreeWidget*> trees;

  struct Window {

    static inline std::vector<Window*> pool;

    std::string name;
    std::string uid;
    bool active = true;

    Window(std::string name) : name(name), uid(("window"+std::to_string(pool.size()))) { pool.push_back(this); }

    void drawFull();

    virtual void draw() {}
    virtual void drawTree() { draw(); }
    virtual void drawEditor() {}

  };

};