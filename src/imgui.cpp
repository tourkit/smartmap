#include "imgui.hpp"
GUI::GUI(GLFWwindow* window) {

  const char* glsl_version = "#version 430";
  ImGui::CreateContext();
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

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

void GUI::draw() {
    
  newframe();

  ImGui::Begin("Uniforms");
  ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);

  for (auto e:elements) {
    
    e.get()->draw();
    e.get()->send();

  }

  ImGui::End();

  render();
  
}

void GUI::add(Element* elem) {
  
    elements.push_back(std::shared_ptr<Element>(elem));

}
