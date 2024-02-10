#include "gui.hpp"



#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"


#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

GUI::GUI(GLFWwindow* window) {

  ImGui::CreateContext();

  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  

  ImGui_ImplGlfw_InitForOpenGL(window, true);

  const char* glsl_version = "#version 430";
  ImGui_ImplOpenGL3_Init(glsl_version);


  auto color_white      = ImVec4(1,1,1,1);
  auto color_orange      = ImVec4(1,.4,0,1);
  auto color_darkergrey  = ImVec4(.1,.1,.1,1);
  auto color_darkgrey  = ImVec4(.2,.2,.2,1);
  auto color_lightgrey   = ImVec4(.25,.25,.25,1);
  auto color_lightergrey   = ImVec4(.4,.4,.4,1);

  ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt,ImVec4(.4,.4,.4,.15));
  ImGui::PushStyleColor(ImGuiCol_TableBorderLight,color_darkergrey);
  ImGui::PushStyleColor(ImGuiCol_TableBorderStrong,color_darkergrey);
  ImGui::PushStyleColor(ImGuiCol_CheckMark,color_white);
  ImGui::PushStyleColor(ImGuiCol_FrameBg,color_darkgrey);
  ImGui::PushStyleColor(ImGuiCol_HeaderHovered,color_lightergrey);
  ImGui::PushStyleColor(ImGuiCol_Header,color_lightergrey);
  ImGui::PushStyleColor(ImGuiCol_Tab,color_lightergrey);
  ImGui::PushStyleColor(ImGuiCol_TabUnfocused,color_lightergrey);
  ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive,color_lightergrey);
  ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,color_lightgrey);
  ImGui::PushStyleColor(ImGuiCol_FrameBgActive,color_lightgrey);
  ImGui::PushStyleColor(ImGuiCol_TitleBg,color_lightgrey);
  ImGui::PushStyleColor(ImGuiCol_TitleBgActive,color_lightgrey);
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab,color_lightgrey);
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered,color_lightgrey);
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive,color_lightgrey);
  ImGui::PushStyleColor(ImGuiCol_SliderGrab,color_lightergrey);
  ImGui::PushStyleColor(ImGuiCol_SliderGrabActive,color_orange);
  ImGui::PushStyleColor(ImGuiCol_Button,color_lightgrey);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered,color_orange);
  ImGui::PushStyleColor(ImGuiCol_TabHovered,color_orange);
  ImGui::PushStyleColor(ImGuiCol_TabActive,color_orange);
  ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // Darker border color
    
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(0,0));
  ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing,10);
  
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

void GUI::Window::drawFull() { 

   {

      if (!active) return;

      bool p_open = true;
      ImGui::Begin((name+"##"+uid).c_str(), &p_open);

      draw();

      ImGui::End();

    }


}

void GUI::draw() { 

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  newframe(); 
  if (draw_gui) {
    ImGui::ShowDemoWindow();
    for (auto window : Window::pool) { window->drawFull(); } 
    

  } 

  render(); 
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