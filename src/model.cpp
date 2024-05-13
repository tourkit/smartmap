#include "model.hpp"
#include "engine.hpp"


#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


Model::Model(File* f, std::string name) : s(name), file(f) {  };

Model::~Model() { }

bool Model::remove(Effector* effector) { return std::erase_if( effectors, [&](std::shared_ptr<Effector> e) { return e.get() == effector; }); }

Effector* Model::addEffector(File* file) {

    if (file->extension != "glsl") { PLOGW << "WARUM :GLSL only BB !!"; return nullptr;}

    auto effector = effectors.emplace_back(std::make_shared<Effector>(file, s.next_name(file->name()))).get();

    s.add(&effector->ref);

    return effector;

}


void Model::fbx(File* file) {

 Assimp::Importer importer;
 Assimp::Exporter exporter;

    const aiScene* scene = importer.ReadFileFromMemory(&file->data[0], file->data.size(), aiProcess_CalcTangentSpace       |
        aiProcess_Triangulate            |
        aiProcess_JoinIdenticalVertices  |
        aiProcess_SortByPType);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        PLOGE << "Failed to load OBJ file: " << file->path << " . " << importer.GetErrorString(); return; }

std::string finale = File::loc()+"/"+file->name()+".fbx";

auto x = exporter.Export(scene, "fbx", finale, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);



}
