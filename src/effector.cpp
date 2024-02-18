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

    std::string argsStr = file->data;
    std::regex argPattern(R"(//\s*([a-zA-Z]+)\s*\((\s*-?\d+(\.\d+)?\s*(,\s*-?\d+(\.\d+)?\s*(,\s*-?\d+(\.\d+)?\s*)?)?)?\))");
    std::sregex_iterator next(argsStr.begin(), argsStr.end(), argPattern);
    std::sregex_iterator end;
    while (next != end) {

        std::smatch match = *next++;
        std::string range;
        int i = 0;
        std::istringstream stream(match[2].str());
        while (std::getline(stream, range, ',')) ranges[match[1]][i++] = std::stof(range);

    }


    args.resize(0);
    
    if (std::regex_search(file->data, match, std::regex(R"(\b(\w+)\s*(?:\(\s*\))?\s*\(\s*((?:\w+\s+\w+\s*(?:,\s*)?)*)\))"))) {

        std::string argsStr = match[2].str();
        
        std::regex argPattern(R"(\b(\w+)\s+(\w+)\s*(?:,\s*)?)");
        std::sregex_iterator iter(argsStr.begin(), argsStr.end(), argPattern);
        std::sregex_iterator end;
        while (iter != end) {
            args.push_back({(*iter)[1].str(),(*iter)[2].str()});
            ++iter;
        }
    }

}