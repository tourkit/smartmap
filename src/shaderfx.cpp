#include "shaderfx.hpp"
#include "file.hpp"
#include <regex>

ShaderFX::ShaderFX()  { }
ShaderFX::ShaderFX(File *file) { import(file); }


std::vector<std::string> ShaderFX::extractArgsFromFunction(const std::string& functionSrc) {
        
        args.resize(0);
        // Regular expression pattern to match function arguments
        std::regex pattern(R"(\b(\w+)\s*(?:\(\s*\))?\s*\(\s*((?:\w+\s+\w+\s*(?:,\s*)?)*)\))");

        std::smatch match;
        if (std::regex_search(functionSrc, match, pattern)) {
            // Extract the arguments from the matched groups
            std::string argsStr = match[2].str();
            std::regex argPattern(R"(\b(\w+)\s+(\w+)\s*(?:,\s*)?)");
            std::sregex_iterator iter(argsStr.begin(), argsStr.end(), argPattern);
            std::sregex_iterator end;
            while (iter != end) {
                args.push_back((*iter)[2].str());
                ++iter;
            }
        }

        return args;
    }



void ShaderFX::import(File *file) {

    this->file = file;

    extractArgsFromFunction(file->data);

}


// //---------
// ShaderFXPtr::ShaderFXPtr(Node *ptr) : Ptr<ShaderFX>((ShaderFX*)ptr) { }
// void ShaderFXPtr::editor() { 
        
//     for (int i=0; i < ptr->args.size(); i++) {
//         float f = 0;
//         ImGui::SliderFloat(ptr->args[i].c_str(), &f, 0, 1);
//     }

// }