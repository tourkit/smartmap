#include "effector.hpp"
#include "file.hpp"
#include "log.hpp"
#include "struct.hpp"
#include <regex>



std::string Effector::source() { if (!definitions.size()) return ""; return definitions.back()->source; }

Effector::Definition::Definition() {

    //

}

Effector::Definition::Definition(File* file) {

    source = (&file->data[0]);

    // extract args

    std::vector<std::pair<std::string,std::string>> args;

    std::regex regex; std::smatch match;

    regex = std::regex(R"(\b(\w+)\s*(?:\(\s*\))?\s*\(\s*((?:\w+\s+\w+\s*(?:,\s*)?)*)\))");
    for (std::sregex_iterator it(source.begin(), source.end(), regex), end; it != end; ++it) {
        std::smatch match = *it;

        std::string nameStr = match[1].str();
        if (strcmp(nameStr.c_str(),file->name().c_str())) continue;
        std::string argsStr = match[2].str();

        std::regex regex(R"(\b(\w+)\s+(\w+)\s*(?:,\s*)?)");
        std::sregex_iterator it_(argsStr.begin(), argsStr.end(), regex);
        std::sregex_iterator end_;
        while (it_ != end_) {
            args.push_back({(*it_)[1].str(),(*it_)[2].str()});
            ++it_;
        }

        break;
    }

    // extract ranges

    std::map<std::string, float[3]> ranges;

    // fetch defaul values a.k.a // xxx(min,max,def)
    int range_count = 0;
    regex = std::regex(R"(//\s*([a-zA-Z]+)\s*\((\s*-?\d*(\.\d*)?\s*(,\s*-?\d*(\.\d*)?\s*(,\s*-?\d*(\.\d*)?\s*)?)?)?\))");
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

    // build Member

    s.name( file->name() );

    s.striding(true);

    for (auto arg : args) {

        if (arg.first == "vec2") s.add<glm::vec2>(arg.second.c_str());
        else if (arg.first == "vec3") s.add<glm::vec3>(arg.second.c_str());
        else if (arg.first == "vec4") s.add<glm::vec4>(arg.second.c_str());

        else if (arg.first == "int") s.add<int>(arg.second.c_str());

        else s.add<float>(arg.second.c_str());

        if (ranges.find(arg.second) != ranges.end()) s.range(ranges[arg.second][0],ranges[arg.second][1],ranges[arg.second][2]);

    }

    // clean source

    size_t pos = 0;
    while ((pos = source.find("//", pos)) != std::string::npos) {

        size_t end = source.find("\n", pos);
        if (end == std::string::npos) end = source.length();
        source.erase(pos, end - pos+1);

    }

    source.resize(source.find_last_of("}")+1);

}

Effector::Effector(std::string name, int wrap, std::vector<Definition*> defs ) : ref(name) { }

Effector::Effector(std::string name, Definition* def ) : Effector(name) { definitions.push_back(def); ref.ref = ( &def->s ); };

Effectable::Effectable(std::string name) : s(name) {  }

void Effector::update() {

    if (wrap) {

        if (ref.ref) bkpref = ref.ref;

        ref.ref = nullptr;

        ref.clear();

        ref.add<int>("id");

        for (int i = 0 ; i < wrap; i++) ref.add<float>("param_"+std::to_string(i));

        return;

    }

    if (bkpref) ref.ref = bkpref;

};

bool Effectable::removeEffector(Effector* effector) {

    return std::erase_if( effectors, [&](std::shared_ptr<Effector> e) { return e.get() == effector; });

}

Effector* Effectable::addEffector(Effector::Definition* def) {

    auto effector = effectors.emplace_back(std::make_shared<Effector>(s.next_name(def->s.name()), def)).get();

    s.add(&effector->ref);

    return effector;

}
