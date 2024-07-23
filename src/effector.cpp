#include "effector.hpp"
#include "file.hpp"
#include "builder.hpp"
#include "struct.hpp"
#include "instance.hpp"
#include "atlas.hpp"
#include "utils.hpp"
#include <regex>
#include <string>


// Effector  ////////////////

Effector::Effector(std::string name) : s(name) {

    s.striding(true);

}

bool Effector::header(Builder* builder, Instance isnt) { return true; }


bool Effector::body(Builder* builder, std::string prepend) {

    builder->current_model += "\t"+call(prepend)+"\n";

    return true;
}


std::string Effector::call(std::string prepend) {

    std::string args;

    for (auto &arg : s.members) args += prepend +"."+ arg->name()+", "; 

    if (args.length()) args.resize(args.size()-2);

    return s.name()+"("+args+");";
    
}


// FeedbackEffector  ////////////////

bool FeedbackEffector::setup(Builder* builder) { 

    
    
    return true; 
    
}

bool FeedbackEffector::body(Builder* builder, std::string prepend) { 

    std::string current;
    
    current += "//\tfloat steps = 5;\n";
    current += "//\tfloat smoothing = .8;\n";
    current += "//\tif (dynamic_ubo[curr].uberLayer1.smartLayer1[int(OBJ)].feedback.intensity != 0) for (float i = 1; i < steps; i++) {\n\n";
    current += "//\t\tif (abs(COLOR)!=vec4(0)) break;\n\n";
    current += "//\t\tfloat angle = dynamic_ubo[curr].uberLayer1.smartLayer1[int(OBJ)].rectangle.orientation;\n";
    current += "//\t\tvec2 size = dynamic_ubo[curr].uberLayer1.smartLayer1[int(OBJ)].rectangle.size;\n";
    current += "//\t\tvec2 pos = dynamic_ubo[curr].uberLayer1.smartLayer1[int(OBJ)].rectangle.pos;\n\n";
    current += "//\t\tvec2 pos2 = dynamic_ubo[last].uberLayer1.smartLayer1[int(OBJ)].rectangle.pos;\n\n";
    current += "//\t\tfloat step = i/steps;\n";
    current += "//\t\t\n\t\t// // 3.14159265359 // 6.2831853072\n";
    current += "//\t\tfloat angle2 = dynamic_ubo[last].uberLayer1.smartLayer1[int(OBJ)].rectangle.orientation;\n";
    current += "//\t\tfloat diff = abs(angle-angle2);\n";
    current += "//\t\tif (diff>.75)  if (angle2>angle) angle2 = -1+angle2; else angle2 = 1+angle2;\n";
    current += "//\t\tif (abs(angle-angle2)<.25) angle = mix(angle,angle2,step);\n";
    current += "//\t\tif (abs(size.x-dynamic_ubo[last].uberLayer1.smartLayer1[int(OBJ)].rectangle.size.x)<.015 && abs(size.y-dynamic_ubo[last].uberLayer1.smartLayer1[int(OBJ)].rectangle.size.y)<.015) size = mix(size,dynamic_ubo[last].uberLayer1.smartLayer1[int(OBJ)].rectangle.size,step);\n";
    current += "//\t\tif (abs(pos.x-pos2.x)<.12 && abs(pos.y-pos2.y)<.12) pos = mix(pos,pos2,step);\n\n";
    current += "//\t\ttic();\n\n";
    current += builder->current_layer+"\n\n";
    current += "//\t\tcolor *= ((steps-i)/steps)*(1-smoothing)+smoothing;\n\n";
    current += "//\t\ttac();\n\t\t\n";
    current += "//\t}\n\n";

    builder->current_layer = current;

    return true;

}


FeedbackEffector::FeedbackEffector() : Effector("feedbackEffector") {

    s.add<float>("intensity");

    
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

    s.clear();

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

Wrappy::Wrappy(std::vector<Effector*> effectors, int count, std::string name) : Effector(name), Effectable("") {

    for (auto x : effectors) addEffector(x);

    attrs(count);

};


// static 
std::string Wrappy::source() {

    std::string args;
    for (auto x : Effector::s.members) args += x->type_name() + " " + x->name() + ", ";

    std::string out = "void " + Effector::s.name() + " ( " + (args.length()?args.substr(0, args.length() - 2):"") + " ) ";

    out += "{\n\n"; 
    
    int id = 0;
    for (auto x : effector_refs) {

        std::string args;

        for (int i = 0; i < x->s.ref()->members.size(); i++) {
            
            args +=  (i<count?Effector::s.members[i+1]->name():"0") + ", ";
            
        }
        
        out += "\tif (id=="+std::to_string(id++)+") { " + x->s.name() + "( " + (args.length()?args.substr(0, args.length() - 2):"") + " ); return; }\n\n";
        
    }

    out += "\n\n}\n\n";
    
    return out;

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

bool Wrappy::setup(Builder* builder) { 

    Effector::s.members[0]->range<int>(0,effector_refs.size()-1,0);
    
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

EffectorRef::EffectorRef(std::string name, Effector* effector ) : s(name), effector(effector) {  s.ref( &effector->s ); };

void EffectorRef::update() {




};

// Effectable  ////////////////
// Effectable  ////////////////
// Effectable  ////////////////
// Effectable  ////////////////

Effectable::~Effectable() {  }

Effectable::Effectable(std::string name) : s(name) {  }

bool Effectable::removeEffector(EffectorRef* effector) {

    return std::erase_if( effector_refs, [&](std::shared_ptr<EffectorRef> e) { return e.get() == effector; });

}

EffectorRef* Effectable::addEffector(Effector* def) {

    auto effector = effector_refs.emplace_back(std::make_shared<EffectorRef>(s.next_name(def->s.name()), def)).get();

    s.add(&effector->s);

    return effector;

}
