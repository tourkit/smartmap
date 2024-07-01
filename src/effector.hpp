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

    Effector(std::string name, Definition* def);

    Effector(std::string name, int wrap = 0, std::vector<Definition*> defs = {});

    virtual std::string source();

    int wrap = 0;

    void update();

private:

    Member* bkpref = nullptr;

};

struct Effectable {

    Struct s;

    Effectable(std::string name = "Effectable" );

    std::vector<std::shared_ptr<Effector>> effectors;
    Effector* addEffector(Effector::Definition* def); // kinda ctor for effectors
    bool removeEffector(Effector* effector);

};
