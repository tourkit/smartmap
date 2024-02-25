#include "effector.hpp"
#include "file.hpp"
#include "log.hpp"
#include <regex>

Effector::Effector()  { }

Effector::Effector(File *file) { import(file); }

void Effector::import(File *file) {

    this->file = file;

    std::smatch match;

    ranges.clear();

    int range_count = 0;
    auto source = file->data;
    std::regex regex(R"(//\s*([a-zA-Z]+)\s*\((\s*-?\d+(\.\d+)?\s*(,\s*-?\d+(\.\d+)?\s*(,\s*-?\d+(\.\d+)?\s*)?)?)?\))");
    std::sregex_iterator next(source.begin(), source.end(), regex);
    std::sregex_iterator end;
    while (next != end) {

        std::smatch match = *next++;
        std::string range;
        int i = 0;
        std::istringstream stream(match[2].str());
        while (std::getline(stream, range, ',')) ranges[match[1]][i++] = std::stof(range);
        range_count++;

    }

    args.resize(0);
    
    if (std::regex_search(source, match, std::regex(R"(\b(\w+)\s*(?:\(\s*\))?\s*\(\s*((?:\w+\s+\w+\s*(?:,\s*)?)*)\))"))) {

        std::string argsStr = match[2].str();
        
        std::regex regex(R"(\b(\w+)\s+(\w+)\s*(?:,\s*)?)");
        std::sregex_iterator iter(argsStr.begin(), argsStr.end(), regex);
        std::sregex_iterator end;
        while (iter != end) {
            args.push_back({(*iter)[1].str(),(*iter)[2].str()});
            ++iter;
        }
    }

}

std::string Effector::source() { 
    
    auto out_code = file->data;

    size_t pos = 0;
    while ((pos = out_code.find("//", pos)) != std::string::npos) {

        size_t end = out_code.find("\n", pos);
        if (end == std::string::npos) end = out_code.length();
        out_code.erase(pos, end - pos+1);

    }

    
    return out_code.c_str(); 
    
    // or...
    // size_t pos = file->data.find("void");
    // return std::string(&file->data[pos]);
    
}