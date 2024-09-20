#include "gui.hpp"
#include <memory>


#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#undef IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"

#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "editor.hpp"
#include "tree.hpp"
#include "window.hpp"
#include "assets/fonts/IconsLucide.h"






void GUI::Window::drawFull() { {

      if (!active) return;

      bool p_open = true;

      ImGuiWindowFlags  flag = 0;ImGuiWindowFlags_MenuBar;ImGuiWindowFlags_NoTitleBar;

      std::string str = name+"###"+uid;


      ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(200,200));
      ImGui::Begin(str.c_str(), &p_open, flag);
      if (!p_open) 
        gui->window->end_of_render_cbs.push_back(std::pair<void*, std::function<void(void*)>>{this, ([&](void* ptr){

           std::erase_if(gui->editors, [ptr](std::shared_ptr<EditorWidget> e) { return e.get() == ptr; });

        })});
      
   ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems && (window->Flags & ImGuiWindowFlags_MenuBar)){     

    ImGui::BeginGroup(); // Backup position on layer 0 // FIXME: Misleading to use a group for that backup/restore
    ImGui::PushID("##menubar");
    ImRect bar_rect = window->MenuBarRect();
    ImRect clip_rect(IM_ROUND(bar_rect.Min.x + window->WindowBorderSize), IM_ROUND(bar_rect.Min.y + window->WindowBorderSize), IM_ROUND(ImMax(bar_rect.Min.x, bar_rect.Max.x - ImMax(window->WindowRounding, window->WindowBorderSize))), IM_ROUND(bar_rect.Max.y));
    clip_rect.ClipWith(window->OuterRectClipped);
    ImGui::PushClipRect(clip_rect.Min, clip_rect.Max, false);
    ImGui::PopClipRect();
    ImGui::PopID();
    ImGui::EndGroup();
      }
      draw();

      ImGui::End();
      ImGui::PopStyleVar();
    }


}

///// GUIGUIGUIGUIGUIG


  static std::vector<Node*>  bkp;
GUI::GUI(::Window* window) : window(window) {

  ImGui::CreateContext();

  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  io.WantCaptureKeyboard = false;

  ImGui_ImplGlfw_InitForOpenGL(window->id, false);
ImGui_ImplGlfw_InstallCallbacks(window->id);
ImGui_ImplGlfw_SetCallbacksChainForAllWindows(true);

  const char* glsl_version = "#version 430";
  ImGui_ImplOpenGL3_Init(glsl_version);


  auto color_white      = ImVec4(1,1,1,1);
  auto color_orange      = ImVec4(.845,.420,0.098,1);
  auto color_darkergrey  = ImVec4(.1,.1,.1,1);
  auto color_darkgrey  = ImVec4(.2,.2,.2,1);
  auto color_lightgrey   = ImVec4(.25,.25,.25,1);
  auto color_lightergrey   = ImVec4(.4,.4,.4,1);



io.Fonts->AddFontDefault();
float baseFontSize = 13.0f; // 13.0f is the size of the default font. Change to the font size you use.
float iconFontSize = baseFontSize * 2.0f / 3.0f; // FontAwesome fonts need to have their sizes reduced by 2.0f/3.0f in order to align correctly

// merge in icons from Font Awesome
static const ImWchar icons_ranges[] = { ICON_MIN_LC, ICON_MAX_16_LC, 0 };
ImFontConfig icons_config; 
icons_config.MergeMode = true; 
icons_config.PixelSnapH = true; 
icons_config.GlyphMinAdvanceX = iconFontSize;
io.Fonts->AddFontFromFileTTF( "assets/fonts/lucide.ttf", iconFontSize, &icons_config, icons_ranges );
// use FONT_ICON_FILE_NAME_FAR if you want regular instead of solid



  ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt,ImVec4(1,1,1,.022));
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
  ImGui::PushStyleColor(ImGuiCol_WindowBg,ImVec4(.1,.1,.1,1));
  ImGui::PushStyleColor(ImGuiCol_TableBorderLight,ImVec4(0,0,0,0));
  ImGui::PushStyleColor(ImGuiCol_TabSelectedOverline,ImVec4(0,0,0,0));
  ImGui::PushStyleColor(ImGuiCol_Button,color_lightgrey);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive,ImVec4(0,0,0,0));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered,color_orange);
  ImGui::PushStyleColor(ImGuiCol_TabHovered,color_orange);
  ImGui::PushStyleColor(ImGuiCol_TabActive,color_orange);
  ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // Darker border color
  ImGui::PushStyleColor(ImGuiCol_HeaderHovered,ImVec4(0,0,0,0));
  ImGui::PushStyleColor(ImGuiCol_HeaderActive,ImVec4(0,0,0,0));
  ImGui::PushStyleColor(ImGuiCol_ScrollbarBg,ImVec4(0,0,0,0));
  ImGui::PushStyleColor(ImGuiCol_HeaderHovered,ImVec4(1,1,1,0.333));
  ImGui::PushStyleColor(ImGuiCol_PopupBg,ImVec4(.254,.254,.254,1));
  ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(.939,.939,.939,1));
  ImGui::PushStyleColor(ImGuiCol_CheckMark,ImVec4(.669,.669,.669,1));

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(0,0));
  ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing,10);
  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,2);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

  trees.push_back(new TreeWidget(this));

  ImGui::GetIO().FontAllowUserScaling = true;

  // editors = bkp;
  for (auto x : bkp) 
      editors.push_back(std::make_shared<EditorWidget>(this,x)); 
  bkp.clear();
  
}



GUI::~GUI() {

  for (auto editor : editors) 
    bkp.push_back(editor->selected);

  auto trees_t = trees;
  for (auto x : trees_t) 
    delete x;


  ImGui::PopStyleVar(3);

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();

  ImGui::DestroyContext();

}

void GUI::newframe() {

  member_count = 0;

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

}



void GUI::draw() {

  if (!draw_gui)
    return;

  newframe();

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // si ca aide .. ?

    for (auto window : GUI::Window::pool) 
      window->drawFull(); 

  render();

}

void GUI::deleteNode(Node* n) {

    if (selected == n)
        selected = nullptr;

    for (auto tree : trees) 
        if (tree->selected == n) 
            tree->selected = nullptr;

    for (auto x : editors) 
        if (x->selected == n) 
            x->selected = nullptr;

    window->end_of_render_cbs.push_back(std::pair<void*, std::function<void(void*)>>{n, ([&](void* ptr){ 
        
        auto n = (Node*)ptr;
        delete n; 

    })});

}

void GUI::render() {

  static int last_framerate = 60;

  if (last_framerate != (int)std::floor(ImGui::GetIO().Framerate) && ImGui::GetIO().Framerate < 59) {

    // PLOGV << "FPS drop @ " << last_framerate+1 << "hz";

    last_framerate = ImGui::GetIO().Framerate;

  }

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




void TestWin::draw() {

    for (int i = 0; i < floats.size(); i++) 
        ImGui::SliderFloat(("float "+std::to_string(i)).c_str(), &floats[i], -30, 30);
      
    for (int i = 0; i < ints.size(); i++) 
        ImGui::SliderInt(("int "+std::to_string(i)).c_str(), &ints[i], -30, 30);

    cb();
    
}
