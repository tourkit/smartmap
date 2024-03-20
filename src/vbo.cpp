#include "vbo.hpp"  

#include "model.hpp"  
#include "file.hpp"  
#include "instance.hpp"  
#include "engine.hpp"  
#include "log.hpp"  

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

VBO::VBO() : Buffer("VBO"), vertices("Vertices", 0), indices("Indices", 0) {

    vertices.add(vertice);
    vertices.striding(true);
    add(vertices);

    indices.add(indice);
    indices.striding(true);
    add(indices);

    create();

}

void VBO::destroy() {

    init = false;

    if (vbo<0) return;

    glDisableVertexAttribArray(0); 
    glDisableVertexAttribArray(1); 
    glDisableVertexAttribArray(2); 
    glDisableVertexAttribArray(3); 

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &vao);

    for (auto m : models) delete m;

}

void VBO::create() {

    destroy();

    glGenBuffers(1, &vbo); glGenBuffers(1, &ibo); glGenVertexArrays(1, &vao);

    init = true;

}

VBO::~VBO()  { destroy(); }

void VBO::update() { Buffer::update(); if (init) upload(); }

void VBO::upload() {

    // tofix

    static std::vector<float> backup_quad = {

        -1,-1, 0,0, 
        1,-1, 1,0, 
        -1,1, 0,1, 
        1,1, 1,1, 

    };

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER,  members[0]->footprint_all(), data.data(), GL_STATIC_DRAW );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, members[1]->footprint_all(), data.data() + members[0]->footprint_all() , GL_STATIC_DRAW );

    PLOGD << data.size();
    PLOGD << members[0]->footprint_all();
    PLOGD << members[1]->footprint_all();
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, 16, (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 16, (GLvoid *) 16);
    glEnableVertexAttribArray(1);
}

void VBO::draw(int count) {

    glBindVertexArray(vao); 

    glDrawElementsInstanced(GL_TRIANGLES, members[1]->footprint_all(), GL_UNSIGNED_INT, 0, count);

}
	

int VBO::import(File *file) {    

    // return 0;  // tofix

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFileFromMemory(&file->data[0], file->data.size(), aiProcess_CalcTangentSpace       | 
        aiProcess_Triangulate            |
        aiProcess_JoinIdenticalVertices  |
        aiProcess_SortByPType);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) PLOGW << "Failed to load OBJ file: " << importer.GetErrorString();


    auto mesh = scene->mMeshes[0];

    int next_indice =  (*this)[0].member->members.size();

    for (int i = 0; i < mesh->mNumVertices; i++) {

        const aiVector3D& vertex = mesh->mVertices[i];
        
        auto v = (*this)[0].push();

        v["Position"].set<glm::vec2>({ vertex.x, vertex.y });
        v["UV"].set<glm::vec2>({ mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
        // v["ID"].set<uint32_t>(0);


    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
      
        const aiFace& face = mesh->mFaces[i];
        auto indice = (*this)[1].push();

        indice[0].set<int>(next_indice+face.mIndices[0]);
        indice[1].set<int>(next_indice+face.mIndices[1]);
        indice[2].set<int>(next_indice+face.mIndices[2]);


    }

    upload();

    // models.push_back(new Model(file, models.size()));

    // return models.size()-1;

    return 0;
    
}