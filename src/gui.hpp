#pragma once


#include <vector>
#include <string>
#include <cstring>

#include "log.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
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


    bool VCharSlider(void* c, int label_id = 0) ;


struct Node;

struct GUI {

  void *dragging = nullptr;

  bool draw_gui = true;

  GUI(GLFWwindow* window);

  ~GUI();

  void newframe();
  void render();

  void draw() ;

  std::vector<EditorWidget*> editors;
  std::vector<TreeWidget*> trees;


  static inline std::vector<Node*> delete_list;

  struct Window {

    static inline std::vector<Window*> pool;

    std::string name;
    std::string uid;
    bool active = true;


    Window(std::string name) : name(name), uid(("window"+std::to_string(pool.size()))) { if(!pool.size()) pool.reserve(20); else if (pool.size()>20) PLOGW << "viens on en parle"; pool.push_back(this); }
    virtual  ~Window() { std::erase_if(pool, [this](Window* w) { return w == this; }); }
    void drawFull();

    virtual void close() { close_list.push_back(this); }

    virtual void draw() {}
    virtual void drawTree() { draw(); }
    virtual void drawEditor() {}
  };

private:
    static inline std::vector<Window*> close_list;
};
