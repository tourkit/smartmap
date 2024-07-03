#pragma once

#include "struct.hpp"

#include <map>
#include <vector>
#include <string>

struct File;
struct Effector {

    Struct s;

    std::string source;

    Effector(File* file);

    Effector();

};

struct EffectorRef  {


    static inline std::map<File*, Effector> effects;

    enum Type { FRAGMENT, VERTEX, COMPUTE } type;

    Struct s;

    std::vector<Effector*> definitions;

    EffectorRef(std::string name, Effector* def);

    EffectorRef(std::string name, int wrap = 0, std::vector<Effector*> defs = {});

    virtual std::string source();

    int wrap = 0;

    void update();

private:

    Member* bkpref = nullptr;

};

struct Effectable {

    Struct s;

    Effectable(std::string name = "Effectable" );

    std::vector<std::shared_ptr<EffectorRef>> effectors;
    EffectorRef* addEffector(Effector* def); // kinda ctor for effectors
    bool removeEffector(EffectorRef* effector);

};
