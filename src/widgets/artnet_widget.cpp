
#include "artnet_widget.hpp"

#include "src/artnet.hpp"

  ArtnetWindow::~ArtnetWindow() { delete artnet; }

  StringsBuffer options;


  ArtnetWindow::ArtnetWindow(Artnet * artnet) : Window("Artnet Universes") { 
    
    this->artnet = artnet; 

  }

  void ArtnetWindow::updateList(int uni) {

    bool exist = false;
    for (auto u : universes) { if (u == uni) { exist = true; break;}}
    if(exist) return;

    std::vector<std::string> stdstr;
    std::vector<const char*> constchar;

    for (auto &u : universes) stdstr.push_back(std::to_string(u)); 
    for (auto &s:stdstr) { constchar.push_back(s.c_str()); }

    options.create(constchar);

  }

  void ArtnetWindow::draw() {


        ImGui::Combo("Select Universe", &uni_current, options.buffer);
        
        auto &dmx = artnet->universes[uni_current];

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2,2));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));

        auto window_width = ImGui::GetWindowWidth();

        int cell_width = window_width / cells_count - 2;

        for (int i = 0; i < 512; i++) {

            ImGui::PushID(i);

            if (ImGui::VSliderScalar("",  ImVec2(cell_width,30),    ImGuiDataType_U8, &dmx.data[i],  &cell_min,   &cell_max,   "")) { 
                
                for (auto &dmx : artnet->universes) { dmx.second.update(); } 
                // fixtureUBO->update(); 
                
            }
            if ((i + 1) % cells_count != 0) { ImGui::SameLine(0); }

            ImGui::PopID();

        }

        ImGui::PopStyleVar(5);


  }

