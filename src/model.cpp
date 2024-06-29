#include "model.hpp"


#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>




#include "effector.hpp"
#include "instance.hpp"

Model::Model(File* f, std::string name) : Modelable(name), file(f) {  };

Model::~Model() { }

void Model::convert(File* file, std::string type) {

 Assimp::Importer importer;
 Assimp::Exporter exporter;

    const aiScene* scene = importer.ReadFileFromMemory(&file->data[0], file->data.size(), aiProcess_CalcTangentSpace       |
        aiProcess_Triangulate            |
        aiProcess_JoinIdenticalVertices  |
        aiProcess_SortByPType);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        PLOGE << "Failed to load OBJ file: " << file->path << " . " << importer.GetErrorString(); return; }

std::string finale = File::loc()+"/"+file->name()+"."+type;

auto x = exporter.Export(scene, type, finale, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

if (x) PLOGE << exporter.GetErrorString();

}




Effectable::Effectable(std::string name) : s(name) {  }

bool Effectable::removeEffector(Effector* effector) {

    return std::erase_if( effectors, [&](std::shared_ptr<Effector> e) { return e.get() == effector; });

}


Effector* Effectable::addEffector(File* file) {

    if (file->extension != "glsl") { PLOGW << "WARUM :GLSL only BB !!"; return nullptr;}

    auto effector = effectors.emplace_back(std::make_shared<Effector>(file, s.next_name(file->name()))).get();

    s.add(&effector->ref);

    return effector;

}

Model* Modelable::addModel(File* f) {

    auto mod = models.emplace_back(std::make_shared<Model>(f, s.next_name(f->name()))).get();

    s.add(&mod->s);

    for (auto x : s.getTop()) { // should be once even if its a for

        x->isBuff()->each([&](Instance& inst){ if (inst.def() == &mod->s) mod->instance = &inst.track(); });

    }

    return mod;

}

bool Modelable::removeModel(Model* model){ return std::erase_if( models, [&](std::shared_ptr<Model> e) { return e.get() == model; }); }
