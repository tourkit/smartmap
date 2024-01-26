
#include "model.hpp"
#include "file.hpp"
#include "shaderfx.hpp"
#include "gui.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Model::Model(File *file) : Ptr<File>{file} { convert();}

void Model::convert() {    

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFileFromMemory(&ptr->data[0], ptr->data.size(), aiProcess_CalcTangentSpace       | 
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

}

ModelPtr::ModelPtr(void* ptr) : Ptr<Model>(ptr) { }
    
Node* ModelPtr::add(Node *node) {

    if (node->is_a<ShaderFX>()) {

        Node::add(new ShaderFXPtr(node));

        return node;
        
    }

    return nullptr;

}    
void ModelPtr::editor() {

    for (auto c : childrens) {
        
        
        if (ImGui::TreeNodeEx(c->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)){ c->editor(); ImGui::TreePop(); }

   } 
}