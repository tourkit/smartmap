#include "gui.hpp"
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
    
  // newframe();

  ImGui::Begin("Uniforms");
  ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);

  for (auto e:elements) {
    
    e.get()->draw();
    e.get()->send();

  }

  ImGui::End();

  // render();
  
}
 
Ref<GUI::Element> GUI::add(Element* elem, ShaderProgram* shader)  {

    elements.push_back(Ref<Element>(elem));
    if (shader) elements.back()->links.insert(shader);
    return elements.back();

}

///////////////////////////////////////


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
 