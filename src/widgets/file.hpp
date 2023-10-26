#include "../gui.hpp"

struct FileWidget : GUI::Window {

        FileWidget() : GUI::Window("Files") { 
        
        
        }
        
        void draw() override {

            if (ImGui::BeginTable("table1", 4)) {

                if (false)
                {
                    ImGui::TableSetupColumn("One");
                    ImGui::TableSetupColumn("Two");
                    ImGui::TableSetupColumn("Twos");
                }

                for (int i = 0; i< File::pool.size(); i++) { 

                    File* file = File::pool[i];

                    ImGui::TableNextColumn();
                    ImGui::Checkbox(("##cb"+std::to_string(i)).c_str(), &file->survey);
                    ImGui::TableNextColumn();
                    ImGui::Text(file->name.c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(file->extension.c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(file->last_modified).c_str());

                }
                
                ImGui::EndTable();
            }
        }

    };