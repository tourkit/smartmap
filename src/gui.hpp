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

  struct Element {

    std::string name;

    float def = 0;
    float min = 0;
    float max = 1;

    float* ptr;

    size_t size = 0;

    std::set<ShaderProgram*> links;

    std::vector<float> data;

    

    void send() {

      for (auto* l:links) {
        
        if (size == 1) l->sendUniform(name.c_str(), *ptr);
        if (size == 2) l->sendUniform(name.c_str(), *ptr, *ptr+1);
        if (size == 3) l->sendUniform(name.c_str(), *ptr, *ptr+1, *ptr+2);
        if (size == 4) l->sendUniform(name.c_str(), *ptr, *ptr+1, *ptr+2, *ptr+3);

      }

    }

    Element(std::string name, size_t size = 1, float def = 0, float min = 0, float max =1, float* ptr = nullptr)
      : name(name), def(def), min(min), max(max), ptr(ptr), size(size) {

      if (ptr) return;

      data.resize(size); 
     
      for (size_t i = 0; i < size; i++) data[i] = def;  // for (auto f:data) f = 1; // marche pas ?!!!

      this->ptr = &data[0];
      
    }

    operator int() { return data[0]; }

    virtual void draw() = 0;
    
  };

  struct Counter : public Element {

    Counter(std::string name) : Element(name) { }

    void draw() override {  data[0]++; }

  };

  struct SliderF : public Element {

    SliderF(std::string name, size_t size = 1, float def = 0, float min = 0, float max = 1, float* ptr = nullptr) 
      : Element(name, size, def, min, max, ptr) { }

    void draw() override { 
      
      if (size == 1) ImGui::SliderFloat(name.c_str(),ptr,min,max); 
      else if (size == 2) ImGui::SliderFloat2(name.c_str(),ptr,min,max); 
      else if (size == 3) ImGui::SliderFloat3(name.c_str(),ptr,min,max); 
      else if (size == 4) ImGui::SliderFloat4(name.c_str(),ptr,min,max); 
      
    }

  };

  struct SliderI : public Element {

    SliderI(std::string name, size_t size = 1, float def = 0, float min = 0, float max = 255, float* ptr = nullptr) 
      : Element(name, size, def, min, max, ptr) { }
      
    void draw() override { 

      std::vector<int> data;

      for (size_t i = 0; i < size; i++) data.push_back(*ptr+i);
      
      if (size == 1) ImGui::SliderInt(name.c_str(),&data[0],min,max); 
      else if (size == 2) ImGui::SliderInt2(name.c_str(),&data[0],min,max); 
      else if (size == 3) ImGui::SliderInt3(name.c_str(),&data[0],min,max); 

      for (size_t i = 0; i < size; i++) ptr[i] = data[i];
      
    }

  };

  struct Node {

    const char * name;
    std::vector<Node*> tree;

  };

  static inline bool tree_open, tree_clicked;
  static inline const char* tree_name;
 
  ImRect RenderTree(Node* n)
  {

      tree_open = ImGui::TreeNode(n->name);
      tree_clicked = ImGui::IsItemClicked();
      tree_name = n->name;

      const ImRect nodeRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

      if (tree_open)
      {
          const ImColor TreeLineColor = ImGui::GetColorU32(ImGuiCol_Text);
          const float SmallOffsetX = 11.0f; //for now, a hardcoded value; should take into account tree indent size
          ImDrawList* drawList = ImGui::GetWindowDrawList();

          ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();
          verticalLineStart.x += SmallOffsetX; //to nicely line up with the arrow symbol
          ImVec2 verticalLineEnd = verticalLineStart;


          if (ImGui::IsItemFocused()) std::cout << n->name <<std::endl;

          for (Node* child : n->tree)
          {
              const float HorizontalTreeLineSize = 8.0f; //chosen arbitrarily
              const ImRect childRect = RenderTree(child);
              const float midpoint = (childRect.Min.y + childRect.Max.y) / 2.0f;
              drawList->AddLine(ImVec2(verticalLineStart.x, midpoint), ImVec2(verticalLineStart.x + HorizontalTreeLineSize, midpoint), TreeLineColor);
              verticalLineEnd.y = midpoint;
          }

          drawList->AddLine(verticalLineStart, verticalLineEnd, TreeLineColor);

          ImGui::TreePop();
      }

      return nodeRect;
  }


  std::vector<std::shared_ptr<Element>> elements;

  GUI(GLFWwindow* window);

  ~GUI();

  void newframe();
  void render();
  void draw();

  Ref<GUI::Element> add(Element* elem, ShaderProgram* shader = nullptr);

  static inline GLint GL_BLEND_MODE_IN = 5;
  static inline GLint GL_BLEND_MODE_OUT = 2;
  static void blendTest();

};

#endif
