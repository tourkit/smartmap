#include "model.hpp"


#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>




#include "effector.hpp"
#include "instance.hpp"

Model::Model(File* f, std::string name) : Effectable(name), file(f) {  };

Model::~Model() { }

void Model::convert(File* file, std::string type) {

    Assimp::Importer importer;
    Assimp::Exporter exporter;

    const aiScene* scene = importer.ReadFileFromMemory(&file->data[0], file->data.size(), aiProcess_CalcTangentSpace       |
        aiProcess_Triangulate            |
        aiProcess_JoinIdenticalVertices  |
        aiProcess_SortByPType);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        PLOGE << "Failed to load OBJ file : " << file->path_v << " . " << importer.GetErrorString(); return; }

    std::string finale = File::loc()+"/"+file->name()+"."+type;

    auto x = exporter.Export(scene, type, finale, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

    if (x) 
        PLOGE << exporter.GetErrorString();

}




Model* Modelable::addModel(File* f) {

    auto mod = models.emplace_back(std::make_shared<Model>(f, next_name(f->name()))).get();

    add(mod);

    if (!instance)
        for (auto x : getTop()) // if no root instance ( per buffer )
            x->each([&](Instance& inst){ 
                if (inst.stl.back().m == this) 
                    instance = new Instance(inst); 
            });
            
    mod->instance = &(new Instance(*instance))->loc(mod);

    return mod;

}

bool Modelable::removeModel(Model* model){ return std::erase_if( models, [&](std::shared_ptr<Model> e) { return e.get() == model; }); }
