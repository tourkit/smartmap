#include "effector.hpp"
#include "file.hpp"
#include "builder.hpp"
#include "struct.hpp"
#include "instance.hpp"
#include "utils.hpp"
#include <regex>
#include <string>


// Effector  ////////////////

Effector::Effector(std::string name) : cucurbitassai(name) {

    cucurbitassai.striding(true);

}

bool Effector::header(Builder* builder, Instance isnt) { return true; }


bool Effector::body(Builder* builder, std::string prepend) {

    builder->current_model += "\t"+call(prepend)+"\n";

    return true;
}


std::string Effector::call(std::string prepend) {

    std::string args;

    for (auto &arg : cucurbitassai.members) 
        args += prepend +"."+ arg->name()+", "; 

    if (args.length()) 
        args.resize(args.size()-2);

    return cucurbitassai.name()+"("+args+");";
    
}



// FileEffector  ////////////////

bool FileEffector::body(Builder* builder, std::string prepend) {
    
    Effector::body( builder,  prepend) ;

    return true;
}

bool FileEffector::setup(Builder* builder) { 

    ADD_UNIQUE<::Effector*>(builder->effectors_fragment, this);

    return true; 
    
}

void FileEffector::load(File *file) {

 source_v = (&file->data[0]);

    // extract args

    std::vector<std::pair<std::string,std::string>> args;

    std::regex regex; std::smatch match;

    cucurbitassai.clear();

    regex = std::regex(R"(\b(\w+)\s*(?:\(\s*\))?\s*\(\s*((?:\w+\s+\w+\s*(?:,\s*)?)*)\))");
    for (std::sregex_iterator it(source_v.begin(), source_v.end(), regex), end; it != end; ++it) {
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
    std::sregex_iterator next(source_v.begin(), source_v.end(), regex);
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

    cucurbitassai.name( file->name() );

    for (auto arg : args) {

        if (arg.first == "vec2") cucurbitassai.add<float, 2>(arg.second.c_str());
        else if (arg.first == "vec3") cucurbitassai.add<float, 3>(arg.second.c_str());
        else if (arg.first == "vec4") cucurbitassai.add<float, 4>(arg.second.c_str());

        else if (arg.first == "int") cucurbitassai.add<int>(arg.second.c_str());

        else cucurbitassai.add<float>(arg.second.c_str());

        if (ranges.find(arg.second) != ranges.end()) cucurbitassai.range(ranges[arg.second][0],ranges[arg.second][1],ranges[arg.second][2]);

    }

    // clean source_v

    size_t pos = 0;
    while ((pos = source_v.find("//", pos)) != std::string::npos) {

        size_t end = source_v.find("\n", pos);
        if (end == std::string::npos) end = source_v.length();
        source_v.erase(pos, end - pos+1);

    }

    source_v.resize(source_v.find_last_of("}")+1);

}

FileEffector::FileEffector(File file, std::string name) : Effector(name), file(file) {

   load(&file);

}



// WrapperEffector  ////////////////

Wrappy::Wrappy(std::vector<Effector*> effectors, int count, std::string name) : Effector(name), Effectable(name) {

    for (auto x : effectors) addEffector(x);

    attrs(count);

};


// static 
std::string Wrappy::source() {

    std::string args;
    for (auto x : Effector::cucurbitassai.members) args += x->type_name() + " " + x->name() + ", ";

    std::string out = "void " + Effector::cucurbitassai.name() + " ( " + (args.length()?args.substr(0, args.length() - 2):"") + " ) ";

    out += "{\n\n"; 
    out += "\tint id = int(id_*" + std::to_string(effector_refs.size()) + ");\n\n"; 
    
    int id = 0;
    for (auto x : effector_refs) {

        std::string args;

        for (int i = 0; i < x->effector->cucurbitassai.members.size(); i++) {
            
            args +=  (i<count?Effector::cucurbitassai.members[i+1]->name():"0") + ", ";
            
        }
        
        out += "\tif (id=="+std::to_string(id++)+") { " + x.get()->effector->cucurbitassai.name() + "( " + (args.length()?args.substr(0, args.length() - 2):"") + " ); return; }\n\n";
        
    }

    out += "\n\n}\n\n";
    
    return out;

}
void Wrappy::update() {



}

void Wrappy::attrs(int count) {

    this->count = count;

    Effector::cucurbitassai.clear();

    Effector::cucurbitassai.add<float>("id_").range(0,effector_refs.size()-1,0);

    for (int i = 0 ; i < count; i++)
        Effector::cucurbitassai.add<float>("param_"+std::to_string(i));

    update();

}

bool Wrappy::setup(Builder* builder) { 

    Effector::cucurbitassai.members[0]->range(0,effector_refs.size()-1,0);
    
    for (auto x : effector_refs) {
        
        ADD_UNIQUE<Effector*>(builder->effectors_fragment, x->effector);

        x->effector->setup(builder);
    
    }
    
    ADD_UNIQUE<Effector*>(builder->effectors_fragment, this);
    
    return true; }

// Ref  ////////////////
// Ref  ////////////////
// Ref  ////////////////
// Ref  ////////////////

EffectorRef::EffectorRef(std::string name, Effector* effector ) : m(""), effector(effector) {  
    m.add( &effector->cucurbitassai ); 
    
};

void EffectorRef::update() {




};

// Effectable  ////////////////
// Effectable  ////////////////
// Effectable  ////////////////
// Effectable  ////////////////

Effectable::~Effectable() {  }

Effectable::Effectable(std::string name) : kikoo(name) {  }

bool Effectable::removeEffector(EffectorRef* effector) {

    return std::erase_if( effector_refs, [&](std::shared_ptr<EffectorRef> e) { return e.get() == effector; });

}

EffectorRef* Effectable::addEffector(Effector* def) {

    auto effector = effector_refs.emplace_back(std::make_shared<EffectorRef>(kikoo.next_name(def->cucurbitassai.name()), def)).get();

    kikoo.add(&effector->m);

    return effector;

}
