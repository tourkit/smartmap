#ifndef IMGUI_H
#define IMGUI_H

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_glfw.h"

#include <vector>
#include <string>

#include "shader.hpp"

struct GUIRenderer { static inline std::vector<GUIRenderer*> pool; GUIRenderer() { pool.push_back(this); } virtual void draw() = 0; };

struct GUI {

  struct Panel : GUIRenderer { void draw() override {} };  

  GUI(GLFWwindow* window);

  ~GUI();

  void newframe();
  void render();

  void loadFont(std::string filename, size_t size = 12);

  static inline int GL_BLEND_MODE_IN = 5;
  static inline int GL_BLEND_MODE_OUT = 2;
  static void blendTest();

};

struct GUIWidget : GUIRenderer {

};
struct GUITable : GUIWidget {

  std::vector<std::string> columns;

  void addColumn(std::string column) {

    columns.push_back(column);

  }

  virtual void drawTable() {}

  void draw() override {

    if (ImGui::BeginTable("table", columns.size(), ImGuiTableFlags_RowBg)) {   

      for (int i = 0; i < columns.size(); i++) ImGui::TableSetupColumn(columns[i].c_str());

      ImGui::TableHeadersRow();

      drawTable();
    
      ImGui::EndTable();

    }

  }

};

#endif
