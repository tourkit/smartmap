#include "gui.hpp"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"


#include <GLFW/glfw3.h>

GUI::GUI(GLFWwindow* window) {

  ImGui::CreateContext();

  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  

  ImGui_ImplGlfw_InitForOpenGL(window, true);

  const char* glsl_version = "#version 430";
  ImGui_ImplOpenGL3_Init(glsl_version);
  
}

GUI::~GUI() {   
  
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();

  ImGui::DestroyContext(); 
 
}

void GUI::newframe() {

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

}

void GUI::render() {

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
      GLFWwindow* backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
  }


}