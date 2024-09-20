

/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */


// many vlayers

// delete vlayer should remove corresponding models

// wrapper in layer def

// feedback in layer def

// freetype

// grid / quantity 

// ----


#include "atlas.hpp"
#include "boilerplate.hpp"
#include "drawcall.hpp"
#include "editor.hpp"
#include "engine.hpp"
#include "builder.hpp"
#include "imgui.h"
#include "layer.hpp"
#include "gui.hpp"
#include "member.hpp"
#include "node.hpp"
#include "utils.hpp"
#include <string>
#include <vector>

// fix Member::adding all occurence set default  ..

//  voir comment DC gere lplusieurs models 




struct iosdflg : GUI::Window {

    using GUI::Window::Window;

    void draw() override {

        using namespace ImGui;

        // treepush

        NewLine();
        NewLine();
        NewLine();

        static int x = 9; 
        static int y = 10;

        // DimWiget(&x, &y);


    }

};

int main() {

    logger.cout(Sev::warning);

    engine.init();

    engine.open("project.json");

    static iosdflg w("iosdflg", engine.gui_v);

    engine.run();



}
