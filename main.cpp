/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */

#include "engine.hpp"

#include "artnet.hpp"
#include "editor.hpp"
#include "imgui.h"
#include "layer.hpp"
#include "member.hpp"


// many vlayers

// delete vlayer should remove corresponding models

// ndi

int main() {

    engine.init();
    
    Editor<Stack>([](Node*node,Stack* stack){


        ImVec2 btn_size = {100,50};
        if (ImGui::Button("layer", btn_size)) {
            node->addOwnr<Layer>();
            
        }
        ImGui::SameLine();
        if (ImGui::Button("uberlayer", btn_size)) {

            node->addOwnr<UberLayer>();

        }

    });

    logger.cout(Sev::warning);

    engine.open("project.json");

    PLOGW << Member::structs.size() << " structs";

    engine.run();

}
