#include "shaderfx.hpp"
#include "gui.hpp"
#include "file.hpp"

ShaderFX::ShaderFX(File *file) : Node(file->filename) { import(file); }

void ShaderFXPTr::editor() { 
        
    for (int i=0; i < ptr->args.size(); i++) {
        float f = 0;
        ImGui::SliderFloat(ptr->args[i].c_str(), &f, 0, 1);
    }

    if (ImGui::InputTextMultiline("code", &ptr->code[0], ptr->code.length(), ImVec2(300,300))) {


    };

}

void ShaderFX::import(File *file) {

    name = file->filename;

    code = file->data;

    extractArgsFromFunction(code);

}