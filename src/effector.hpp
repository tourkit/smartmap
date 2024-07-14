#pragma once

#include "struct.hpp"

#include <vector>
#include <string>

struct File;
struct Builder;

struct Effector {

    Struct s;

    enum Type { FRAGMENT, VERTEX, COMPUTE } type;

    virtual bool setup(Builder* builder) = 0;

    virtual bool body(Builder* builder, Instance isnt);

    virtual bool header(Builder* builder, Instance isnt);
    
    virtual void post(Builder* builder) {};

    virtual void update() {}

    std::string source_v;

    virtual std::string source() { return source_v; }

    Effector(std::string name = "effector");

};

struct FeedbackEffector : Effector {

    FeedbackEffector();

    bool setup(Builder* builder) override;
    
    bool body(Builder* builder, Instance isnt) override;

};

struct FileEffector : Effector {

    File file;

    std::string source;

    FileEffector(File file, std::string name = "fileEffector");
    
    void load(File* file);

    bool setup(Builder* builder) override;
    
    bool body(Builder* builder, Instance isnt) override;

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

    std::vector<std::shared_ptr<EffectorRef>> effector_refs;
    EffectorRef* addEffector(Effector* effector); // kinda ctor for effectors
    bool removeEffector(EffectorRef* ref);

};


struct Wrappy : Effector, Effectable {

    bool setup(Builder* builder) override;

    Wrappy(std::vector<Effector*> effectors = {}, int count = 3, std::string name = "wrapperEffector");

    int count;

    void attrs(int count);

    void update() override;

    std::string source() override;

};

// struct Feedback : Wrappy {

//     bool setup(ShaderProgram* shader) override;

// };
