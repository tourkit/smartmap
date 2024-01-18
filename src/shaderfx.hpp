#pragma once


#include "node.hpp"
#include "file.hpp"
#include "shader.hpp"

struct ShaderFX : Node  {

    ShaderType type;

    std::vector<float> uniforms;

    std::string code;

    ShaderFX(std::string filename) { import(filename); }
    ShaderFX(File *file) { import(file); }

    void import(std::string filename) {

        File file("assets/shaders/"+filename);

        import(&file);

    }

    void import(File *file) {

        name = file->name;

        code = file->data;

        extractArgsFromFunction(code);

    }

   std::vector<std::string> args;
    std::vector<std::string> extractArgsFromFunction(const std::string& functionSrc) {
    args.resize(0);
    // Regular expression pattern to match function arguments
    std::regex pattern(R"(\b(\w+)\s*(?:\(\s*\))?\s*\(\s*((?:\w+\s+\w+\s*(?:,\s*)?)*)\))");

    std::smatch match;
    if (std::regex_search(functionSrc, match, pattern)) {
        // Extract the arguments from the matched groups
        std::string argsStr = match[2].str();
        std::regex argPattern(R"(\b(\w+)\s+\w+\s*(?:,\s*)?)");
        std::sregex_iterator iter(argsStr.begin(), argsStr.end(), argPattern);
        std::sregex_iterator end;
        while (iter != end) {
            args.push_back((*iter)[1].str());
            ++iter;
        }
    }

    return args;
}
    
};

