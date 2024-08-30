#include "gui.hpp"

#include "editor.hpp"
#include "tree.hpp"
#include "layer.hpp"
#include "vbo.hpp"


#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"


#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui_internal.h"

bool VCharSlider(void* c, int label_id) {

        static const char* format = "%d";
        static ImVec2 size = ImVec2(22,30);
        static uint8_t min = 0;
        static uint8_t max = 255;
        static const char* label_ = "##vcs";
        std::string label = label_+std::to_string(label_id);

        using namespace ImGui;


        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label.c_str());

        const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + size);
        const ImRect bb(frame_bb.Min, frame_bb.Max);

        const bool temp_input_allowed = 1;
        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(frame_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0)) return false;

        // const bool hovered = ItemHoverable(frame_bb, id);
        // const bool clicked = hovered && IsMouseClicked(0, id);
        // if (clicked || g.NavActivateId == id)
        // {
        //     if (clicked)
        //         SetKeyOwner(ImGuiKey_MouseLeft, id);
        //     SetActiveID(id, window);
        //     SetFocusID(id, window);
        //     FocusWindow(window);
        //     g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Up) | (1 << ImGuiDir_Down);
        // }

        const bool hovered = ItemHoverable(frame_bb, id);
        bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);
        if (!temp_input_is_active)
        {
            // Tabbing or CTRL-clicking on Slider turns it into an input box
            const bool input_requested_by_tabbing = temp_input_allowed && (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
            const bool clicked = hovered && IsMouseClicked(0, id);
            const bool make_active = (input_requested_by_tabbing || clicked || g.NavActivateId == id);
            if (make_active && clicked)
                SetKeyOwner(ImGuiKey_MouseLeft, id);
            if (make_active && temp_input_allowed)
                if (input_requested_by_tabbing || (clicked && g.IO.KeyCtrl) || (g.NavActivateId == id && (g.NavActivateFlags & ImGuiActivateFlags_PreferInput)))
                    temp_input_is_active = true;

            if (make_active && !temp_input_is_active)
            {
                SetActiveID(id, window);
                SetFocusID(id, window);
                FocusWindow(window);
                g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
            }
        }

        if (temp_input_is_active)
        {
            // Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
            const bool is_clamp_input = ( ImGuiSliderFlags_AlwaysClamp) != 0;


            return TempInputScalar(frame_bb, id, label.c_str(), ImGuiDataType_U8, c, format, &min, &max);

        }


        PushStyleVar(ImGuiStyleVar_GrabMinSize, 0);

        PushID(label_id);


        // Draw frame
        const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
        RenderNavHighlight(frame_bb, id);
        RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);

        // Slider behavior
        ImRect grab_bb;
        const bool value_changed = SliderBehavior(frame_bb, id, ImGuiDataType_U8, c, &min, &max, "",  ImGuiSliderFlags_Vertical, &grab_bb);

        grab_bb.Max.y=frame_bb.Max.y-2;
        // grab_bb.Min.y+=style.GrabMinSize;

        if (value_changed)
            MarkItemEdited(id);

        // Render grab
        if (grab_bb.Max.y > grab_bb.Min.y)
            window->DrawList->AddRectFilled(grab_bb.Min, grab_bb.Max, GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

        PushStyleColor(ImGuiCol_Text, ImVec4(.05,.05,.05,1));
        std::string  lid = std::to_string(label_id);
        RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, &(*lid.begin()), &(*lid.end()), NULL, ImVec2(0.5f, 0.0f));
        PopStyleColor();

        // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
        // For the vertical slider we allow centered text to overlap the frame padding
        char value_buf[64];
        const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), ImGuiDataType_U8, c, format);
        RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y+14), frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.0f));


        PopID();
        PopStyleVar(1);

        return value_changed;
    }


void GUI::Window::drawFull() { {

      if (!active) return;

      bool p_open = true;

      ImGuiWindowFlags  flag = 0;ImGuiWindowFlags_MenuBar;ImGuiWindowFlags_NoTitleBar;

      std::string str = name+"###"+uid;

      ImGui::Begin(str.c_str(), &p_open, flag);

      if (!p_open) close();



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

    }


}

///// GUIGUIGUIGUIGUIG


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
  ImGui::PushStyleColor(ImGuiCol_HeaderHovered,ImVec4(0,0,0,0));
  ImGui::PushStyleColor(ImGuiCol_HeaderActive,ImVec4(0,0,0,0));

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(0,0));
  ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing,10);

trees.push_back(new TreeWidget());

    ImGui::GetIO().FontAllowUserScaling = true;

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

void GUI::draw() {

  newframe();

  if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_S)) engine.save();
  if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_I)) engine.gui->draw_gui = !engine.gui->draw_gui;
  if (ImGui::IsKeyDown(ImGuiKey_LeftShift) && ImGui::IsKeyPressed(ImGuiKey_Escape)) exit(0);

  if (draw_gui) {

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // si ca aide .. ?

    for (auto window : Window::pool) 
      window->drawFull(); 

    for (auto x : close_list) 
      delete x;
    close_list.resize(0);

  }

  render();



        for (auto x : rename_list) {

          x.first->name(x.second);

        }
        
        rename_list.clear();

        for (auto x : delete_list) {

            auto parent = x->parent();

            for (auto editor : engine.gui->editors) {

              bool found = false;

              auto is = editor->selected;

              if (is) {

                if (is == x) found = true;

                is = is->parent();

              }

              if (found) editor->selected = engine.tree->childrens[0];

            }

            delete x;

            if (parent) parent->update(); // useless cause already in Node::Dtor na ?
        }

        delete_list.clear();


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
