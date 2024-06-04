#include "model.hpp"
#include "engine.hpp"


#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


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
