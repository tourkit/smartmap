#pragma once

#include "node.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Model : Node {

    std::vector<std::array<float,2>> vertices;

    std::vector<std::array<float,2>> uvs;

    std::vector<std::array<uint32_t,3>> indices;

    Model(std::string path = "quad.obj") {  import(path);}
    
    Model(File *file) {  import(file);}

    void import(std::string path) { File file(path); import(&file); }
       
    void import(File *file) {    

        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFileFromMemory(&file->data[0], file->data.size(), aiProcess_CalcTangentSpace       | 
            aiProcess_Triangulate            |
            aiProcess_JoinIdenticalVertices  |
            aiProcess_SortByPType);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) PLOGW << "Failed to load OBJ file: " << importer.GetErrorString();

        auto mesh = scene->mMeshes[0];

        for (int i = 0; i < mesh->mNumVertices; i++) {

            const aiVector3D& vertex = mesh->mVertices[i];

            std::array<char,20> data;

            vertices.push_back({vertex.x, vertex.y});
            uvs.push_back({mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y});
        }
        
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            
            const aiFace& face = mesh->mFaces[i];

            indices.push_back({face.mIndices[0], face.mIndices[1], face.mIndices[2]});

        }

        this->name = file->name;

    }

};





