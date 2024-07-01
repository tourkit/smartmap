#pragma once

#include "struct.hpp"

#include <map>
#include <vector>
#include <string>

struct File;

struct Effector  {

    struct Definition {

        Struct s;

        std::string source;

        Definition(File* file);

        Definition();

    };

    static inline std::map<File*, Definition> effects;

    enum Type { FRAGMENT, VERTEX, COMPUTE } type;

    Struct ref;

    std::vector<Definition*> definitions;

    Effector(Definition* def, std::string name);

    Effector(std::string name);

    virtual std::string source();

    int wrap = 0;

    void update();

};

struct Effectable {

    Struct s;

    Effectable(std::string name = "Effectable" );

    std::vector<std::shared_ptr<Effector>> effectors;
    Effector* addEffector(Effector::Definition* def); // kinda ctor for effectors
    bool removeEffector(Effector* effector);

};
