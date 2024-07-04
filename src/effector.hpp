#pragma once

#include "struct.hpp"

#include <map>
#include <vector>
#include <string>

struct File;
struct ShaderProgram;

struct Effector {

    Struct s;

    enum Type { FRAGMENT, VERTEX, COMPUTE } type;

    virtual bool setup(ShaderProgram* shader) = 0;

    virtual void update() {}

    virtual std::string source() { return ""; }

    Effector(std::string name = "effector");

};

struct FileEffector : Effector {

    File file;

    std::string source;

    FileEffector(File file, std::string name = "fileEffector");

    bool setup(ShaderProgram* shader) override;

};



struct EffectorRef  {

    Struct s;

    Effector* effector;

    EffectorRef(std::string name, Effector* effector);

    void update();

};

struct Effectable {

    Struct *s_;

    void s(Struct* s_);

    bool owned = true;

    Effectable(Struct* s_);
    Effectable(std::string name = "Effectable" );
    ~Effectable();

    std::vector<std::shared_ptr<EffectorRef>> refs;
    EffectorRef* addEffector(Effector* effector); // kinda ctor for effectors
    bool removeEffector(EffectorRef* ref);

};


struct Wrappy : Effector, Effectable {

    bool setup(ShaderProgram* shader) override;

    Wrappy(std::vector<Effector*> effectors = {}, int count = 3, std::string name = "wrapperEffector");

    int count;

    void attrs(int count);

    void update() override;

    std::string source() override;

};

// struct Feedback : Wrappy {

//     bool setup(ShaderProgram* shader) override;

// };
