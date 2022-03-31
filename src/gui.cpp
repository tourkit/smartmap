#include "gui.hpp"

GUI::GUI(GLFWwindow* window) {

  const char* glsl_version = "#version 430";
  ImGui::CreateContext();

  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; 

  io.Fonts->AddFontFromFileTTF("C:\\projects\\cpp\\smartmap\\smartmap\\include\\vendor\\imgui\\misc\\fonts\\DroidSans.ttf", 20);

  ImGui::GetStyle().WindowBorderSize = 0;
  ImGui::GetStyle().WindowPadding = ImVec2(0,2);
  ImGui::GetStyle().FramePadding = ImVec2(0,0);
  ImGui::GetStyle().CellPadding = ImVec2(0,0);
  ImGui::GetStyle().ItemSpacing = ImVec2(0,5);
  ImGui::GetStyle().ItemInnerSpacing = ImVec2(5,0);
  ImGui::GetStyle().IndentSpacing = 15;
  ImGui::GetStyle().ScrollbarSize = 20;

  ImGui_ImplGlfw_InitForOpenGL(window, true);
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

  newframe();

  for(auto* renderer : GUIRenderer::pool) renderer->draw();

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


std::vector<GLenum> GL_BLEND_MODES = {

    GL_ZERO,
    GL_ONE,
    GL_SRC_COLOR,
    GL_ONE_MINUS_SRC_COLOR,	
    GL_DST_COLOR,
    GL_ONE_MINUS_DST_COLOR,
    GL_SRC_ALPHA,
    GL_ONE_MINUS_SRC_ALPHA,
    GL_DST_ALPHA,
    GL_ONE_MINUS_DST_ALPHA,
    GL_CONSTANT_COLOR,
    GL_ONE_MINUS_CONSTANT_COLOR,
    GL_CONSTANT_ALPHA,
    GL_ONE_MINUS_CONSTANT_ALPHA,
    GL_SRC_ALPHA_SATURATE,
    GL_SRC1_COLOR,
    GL_ONE_MINUS_SRC1_COLOR,
    GL_SRC1_ALPHA,
    GL_ONE_MINUS_SRC1_ALPHA

};


void GUI::blendTest() {

  ImGui::SliderInt("GL_BLEND_MODE_IN",&GL_BLEND_MODE_IN,0,GL_BLEND_MODES.size());
  ImGui::SliderInt("GL_BLEND_MODE_OUT",&GL_BLEND_MODE_OUT,0,GL_BLEND_MODES.size());
  glBlendFunc(GL_BLEND_MODES[GL_BLEND_MODE_IN], GL_BLEND_MODES[GL_BLEND_MODE_OUT]); // OR  

}
 