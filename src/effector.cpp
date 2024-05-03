#include "effector.hpp"
#include "file.hpp"
#include "log.hpp"
// #include "engine.hpp"
#include "node.hpp"
#include "struct.hpp"
#include <regex>

Effector::Definition& Effector::get(File * file) { if (effects.find(file) == effects.end()) init(file); return effects[file]; }

Effector::Effector(File* f, std::string name ) : ref(name), file(file) { ref.add( &get(f).s ); };

void Effector::init(File* file) {

    auto &def = get(file);

    const char* data = (&file->data[0]);

    def.ranges.clear();
    def.args.resize(0);

    std::smatch match;


    int range_count = 0;
    std::string source = data;
    std::regex regex(R"(//\s*([a-zA-Z]+)\s*\((\s*-?\d*(\.\d*)?\s*(,\s*-?\d*(\.\d*)?\s*(,\s*-?\d*(\.\d*)?\s*)?)?)?\))");
    std::sregex_iterator next(source.begin(), source.end(), regex);
    std::sregex_iterator end;
    while (next != end) {

        std::smatch match = *next++;
        std::string range;
        int i = 0;
        std::istringstream stream(match[2].str());
        while (std::getline(stream, range, ',')) def.ranges[match[1]][i++] = std::stof(range);
        range_count++;

    }


    if (std::regex_search(source, match, std::regex(R"(\b(\w+)\s*(?:\(\s*\))?\s*\(\s*((?:\w+\s+\w+\s*(?:,\s*)?)*)\))"))) {

        std::string argsStr = match[2].str();

        std::regex regex(R"(\b(\w+)\s+(\w+)\s*(?:,\s*)?)");
        std::sregex_iterator iter(argsStr.begin(), argsStr.end(), regex);
        std::sregex_iterator end;
        while (iter != end) {
            def.args.push_back({(*iter)[1].str(),(*iter)[2].str()});
            ++iter;
        }
    }

    Struct* s = Struct::exist(file->name());

    if (!s) {

        // should check better like arg comparisons and strideness

        s = &Struct::create(file->name());

        s->striding(true);

        for (auto arg : def.args) {

            if (arg.first == "vec2") s->add<glm::vec2>(arg.second.c_str());
            else if (arg.first == "vec3") s->add<glm::vec3>(arg.second.c_str());
            else if (arg.first == "vec4") s->add<glm::vec4>(arg.second.c_str());

            else if (arg.first == "int") s->add<int>(arg.second.c_str());

            else s->add<float>(arg.second.c_str());

            if (def.ranges.find(arg.second) != def.ranges.end()) s->range(def.ranges[arg.second][0],def.ranges[arg.second][1],def.ranges[arg.second][2]);

        }

    }

}

std::string Effector::source(File* file) {

    std::string out_code = &file->data[0];

    size_t pos = 0;
    while ((pos = out_code.find("//", pos)) != std::string::npos) {

        size_t end = out_code.find("\n", pos);
        if (end == std::string::npos) end = out_code.length();
        out_code.erase(pos, end - pos+1);

    }

    out_code.resize(out_code.find("}")+1);

    return out_code.c_str()+out_code.find("void");


}
