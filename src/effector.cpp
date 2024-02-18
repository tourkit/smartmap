#include "effector.hpp"
#include "file.hpp"
#include "log.hpp"
#include <regex>

Effector::Effector()  { }

Effector::Effector(File *file) { import(file); }

void Effector::import(File *file) {

    this->file = file;

    std::smatch match;

    // ranges.empty();

    if (std::regex_search(file->data, match, std::regex(R"(//\s*([a-zAS-Z]+)\s*\((\s*-?\d+(\.\d+)?\s*(,\s*-?\d+(\.\d+)?\s*(,\s*-?\d+(\.\d+)?\s*)?)?)\))"))) {

        std::string range;
        int i = 0;
        // while (std::getline(std::istringstream(match[2].str()), range, ',')) ranges[match[1]][i++] = std::stof(range);

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