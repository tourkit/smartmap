#include "effector.hpp"
#include "file.hpp"
#include "builder.hpp"
#include "log.hpp"
#include "struct.hpp"
#include "instance.hpp"
#include "atlas.hpp"
#include <regex>


// Effector  ////////////////

Effector::Effector(std::string name) : s(name) {


}


// AtlasEffector  ////////////////

bool AtlasEffector::setup(Builder* builder) { 

    builder->samplers[0] = "medias";
    
    return true; 
    
}


AtlasEffector::AtlasEffector(Atlas* atlas) : Effector("atlasEffector"), atlas(atlas) {

    // if (!atlas->medias->def()->size()) return;

    source_v =  "//id(0,10,0)\n\n";
    source_v += "void atlas(float id_) {\n\n";
    source_v +=     "\tint id = int(id_*10);\n\n";
    source_v +=     "\tvec2 tuv = uv;\n\n";
    source_v +=     "\ttuv *= static_ubo.media[id].size;\n";
    source_v +=     "\ttuv += static_ubo.media[id].pos;\n";
    source_v +=     "\tcolor *= texture(medias, tuv);\n\n";
    source_v += "}\n\n\n\n";

}


// FileEffector  ////////////////

bool FileEffector::setup(Builder* builder) { 
    
    builder->effectors_fragment.push_back(this);
    // for (auto x : effec) 
    return true; 
    
}


FileEffector::FileEffector(File file, std::string name) : Effector(name), file(file) {

    method = (&file.data[0]);

    // extract args

    std::vector<std::pair<std::string,std::string>> args;

    std::regex regex; std::smatch match;

    regex = std::regex(R"(\b(\w+)\s*(?:\(\s*\))?\s*\(\s*((?:\w+\s+\w+\s*(?:,\s*)?)*)\))");
    for (std::sregex_iterator it(method.begin(), method.end(), regex), end; it != end; ++it) {
        std::smatch match = *it;

        std::string nameStr = match[1].str();
        if (strcmp(nameStr.c_str(),file.name().c_str())) continue;
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
    std::sregex_iterator next(method.begin(), method.end(), regex);
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

    s.name( file.name() );

    s.striding(true);

    for (auto arg : args) {

        if (arg.first == "vec2") s.add<glm::vec2>(arg.second.c_str());
        else if (arg.first == "vec3") s.add<glm::vec3>(arg.second.c_str());
        else if (arg.first == "vec4") s.add<glm::vec4>(arg.second.c_str());

        else if (arg.first == "int") s.add<int>(arg.second.c_str());

        else s.add<float>(arg.second.c_str());

        if (ranges.find(arg.second) != ranges.end()) s.range(ranges[arg.second][0],ranges[arg.second][1],ranges[arg.second][2]);

    }

    // clean method

    size_t pos = 0;
    while ((pos = method.find("//", pos)) != std::string::npos) {

        size_t end = method.find("\n", pos);
        if (end == std::string::npos) end = method.length();
        method.erase(pos, end - pos+1);

    }

    method.resize(method.find_last_of("}")+1);

}



// WrapperEffector  ////////////////

Wrappy::Wrappy(std::vector<Effector*> effectors, int count, std::string name) : Effector(name), Effectable("") {

    for (auto x : effectors) addEffector(x);

    attrs(count);

};

std::string Wrappy::source() {

return "//wrappy(){}";

}
void Wrappy::update() {



}

void Wrappy::attrs(int count) {

    this->count = count;

    Effector::s.clear();

    Effector::s.add<int>("id").range(0,effector_refs.size()-1,0);

    for (int i = 0 ; i < count; i++)
        Effector::s.add<float>("param_"+std::to_string(i));

    update();

}

bool Wrappy::setup(Builder* builder) { return true; }

// Ref  ////////////////
// Ref  ////////////////
// Ref  ////////////////
// Ref  ////////////////

EffectorRef::EffectorRef(std::string name, Effector* effector ) : s(name), effector(effector) {  s.ref( &effector->s ); };

void EffectorRef::update() {




};

// Effectable  ////////////////
// Effectable  ////////////////
// Effectable  ////////////////
// Effectable  ////////////////

Effectable::~Effectable() { if (owned) delete s_; }

Effectable::Effectable(std::string name) : s_(new Struct(name)) {  }

Effectable::Effectable(Struct* s_) : s_(s_), owned(false) {  }

void Effectable::s(Struct* s_) { this->s_ = s_; if (owned) delete s_; owned = false; }

bool Effectable::removeEffector(EffectorRef* effector) {

    return std::erase_if( effector_refs, [&](std::shared_ptr<EffectorRef> e) { return e.get() == effector; });

}

EffectorRef* Effectable::addEffector(Effector* def) {

    auto effector = effector_refs.emplace_back(std::make_shared<EffectorRef>(s_->next_name(def->s.name()), def)).get();

    s_->add(&effector->s);

    return effector;

}
