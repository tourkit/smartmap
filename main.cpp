/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */

#include "engine.hpp"

#include "artnet.hpp"
#include "editor.hpp"
#include "imgui.h"
#include "layer.hpp"

// restore orientation
// make dmx work 
// many vlayers

int main() {

    engine.init();

    NODE<Artnet>::on(Node::CHANGE, [&](Node* n, Artnet* an) {

        PLOGW << "dlsdkffds";

    });
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

    engine.run();

}
