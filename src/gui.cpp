#include "smartmap.hpp"

GUI::GUI(GLFWwindow* window) {

  const char* glsl_version = "#version 430";
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
  ImGuiIO& io = ImGui::GetIO();

// Enable docking
io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
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
  ImGui::SliderInt("GL_B2LEND_MODE_OUT",&GL_BLEND_MODE_OUT,0,GL_BLEND_MODES.size());
  glBlendFunc(GL_BLEND_MODES[GL_BLEND_MODE_IN], GL_BLEND_MODES[GL_BLEND_MODE_OUT]); // OR  

}
 
static int  min = 0, max = 255, cells_count = 48;

 void GUI::draw2() {


// return;
        newframe();  
        ImGui::ShowDemoWindow();
        for (auto dmx : sm.artnet->universes) {

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2,2));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));

            ImGui::Begin(std::string("Artnet Universe "+std::to_string(dmx.first)).c_str());
                
                auto window_width = ImGui::GetWindowWidth();

                int cell_width = window_width / cells_count - 2;


                for (int i = 0; i < 512; i++) {

                    ImGui::PushID(i);

                    ImGui::VSliderScalar("",  ImVec2(cell_width,30),    ImGuiDataType_U8, &dmx.second.raw[i],  &min,   &max,   "");

                    if ((i + 1) % cells_count != 0) ImGui::SameLine(0);


                    ImGui::PopID();

                }


            ImGui::End();

            ImGui::PopStyleVar(5);

            break;

        }


        ImGui::Begin("VIEW");

    ImGui::Text(std::to_string(Texture::pool.size()).c_str());

    // std::cout << VBO::pool.size() << std::endl;
    if (ImGui::Button("UPDATE")) VBO::pool[0]->update();
    ImGui::SameLine();
    if (ImGui::Button("RESET")) VBO::pool[0]->reset();
    ImGui::SameLine();
    if (ImGui::Button("DESTROY")) VBO::pool[0]->destroy();
    ImGui::SameLine();
    if (ImGui::Button("CREATE")) VBO::pool[0]->import(VBO::pool[0]->path);

    ImGui::Separator();

            
    // Draw lines
        for (int i = 0; i < Texture::pool.size(); i++) {

            ImGui::Text(("width: " + std::to_string(Texture::pool[i]->width)).c_str());
            ImGui::Text(("height: " + std::to_string(Texture::pool[i]->height)).c_str());
            
            float ratio = Texture::pool[i]->height/(float)Texture::pool[i]->width;
            auto nw = std::min(Texture::pool[i]->width,(GLuint)512);

            ImGui::Image((void*)(intptr_t)(ImTextureID)(uintptr_t)Texture::pool[i]->id, ImVec2(nw,nw*ratio));

            ImGui::PushID(i+100);

            ImGui::PopID();
            ImGui::Separator();
        }


        ImGui::End();
        ImGui::Begin("KTRL");
        
            // ImGui::SetWindowFontScale(1.5);
            
            uint16_t min = 1, max = 2560;

            if (ImGui::DragScalarN("winsize", ImGuiDataType_U16,  &sm.window->width,  2, 1, &min,&max)) sm.window->updateSize();
            if (ImGui::DragScalarN("winpos", ImGuiDataType_U16,  &sm.window->offset_x,  2, 1, &min,&max)) sm.window->updatePos();

            ImGui::Separator();

            // if (ImGui::InputText(" tex", (char*)&sm.tex->path[0], IM_ARRAYSIZE((char*)&sm.tex->path[0]))) sm.tex->reset();
            // if (ImGui::InputText(" frag", (char*)&sm.basic->paths[1][0], IM_ARRAYSIZE((char*)&sm.basic->paths[1][0]))) sm.basic->reset();
            ImGui::Text((std::to_string(ImGui::GetIO().Framerate )+" FPS").c_str());

        ImGui::End();
        
        ImGui::Begin("FixtureUBO");
        // for (int i = 0; i < 20; i++) ImGui::SliderScalar(std::to_string(i).c_str(), ImGuiDataType_U8, (uint8_t*)(&sm.artnet->universes[0].raw[i]),  &min,   &max,   "");
        for (int i = 0; i < 20; i++) ImGui::SliderFloat(("uniform "+std::to_string(i)).c_str(), &sm.fixtureUBO->data[i], 0, 1);
        ImGui::End();

        render();


 }