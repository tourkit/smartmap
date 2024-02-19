#include "vbo.hpp"  

#include "model.hpp"  
#include "file.hpp"  
#include "entry.hpp"  

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

VBO::VBO() {  

    vertices = addObj(vertices_s);

    indices = addObj(indices_s);

    create();
}

void VBO::destroy() {

    if (vbo<0) return;

    glDisableVertexAttribArray(0); 
    glDisableVertexAttribArray(1); 
    glDisableVertexAttribArray(2); 
    glDisableVertexAttribArray(3); 

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &vao);

}

void VBO::create() {

    destroy();

    glGenBuffers(1, &vbo); glGenBuffers(1, &ibo); glGenVertexArrays(1, &vao);

}

VBO::~VBO()  { destroy(); }

void VBO::update() {upload();}

void VBO::upload() {

    static std::vector<float> backup_quad = {

        -1,-1, 0,0, 0,
        1,-1, 1,0, 0,
        -1,1, 0,1, 0,
        1,1, 1,1, 0,

    };

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,  vertices->size(), vertices->data(), GL_STATIC_DRAW );
    // glBufferData(GL_ARRAY_BUFFER,  vertices->size(), &backup_quad[0], GL_STATIC_DRAW );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size(), indices->data(), GL_STATIC_DRAW );

    // make this parametric from Object vertices definition
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, vertices->s->size, (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, vertices->s->size, (GLvoid *) (2*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_TRUE, vertices->s->size, (GLvoid *) (4*sizeof(float)));
    glEnableVertexAttribArray(3);

}

void VBO::draw(int count) {

    glBindVertexArray(vao); 

    glDrawElementsInstanced(GL_TRIANGLES, indices->size(), GL_UNSIGNED_INT, 0, count);

}
	

int VBO::import(Model *model) {    

    File* file = model->file;

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFileFromMemory(&file->data[0], file->data.size(), aiProcess_CalcTangentSpace       | 
        aiProcess_Triangulate            |
        aiProcess_JoinIdenticalVertices  |
        aiProcess_SortByPType);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) PLOGW << "Failed to load OBJ file: " << importer.GetErrorString();

    auto mesh = scene->mMeshes[0];

    for (int i = 0; i < mesh->mNumVertices; i++) {

        const aiVector3D& vertex = mesh->mVertices[i];
        
        auto v = vertices->push();

        v["Position"]["x"].set<float>(vertex.x);
        v["Position"]["y"].set<float>(vertex.y);

        v["UV"]["x"].set<float>(mesh->mTextureCoords[0][i].x);
        v["UV"]["y"].set<float>(mesh->mTextureCoords[0][i].y);

        v["ID"][0].set<float>(0);


    }
    
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        
        const aiFace& face = mesh->mFaces[i];

        auto indices = this->indices->push();

        indices[0][0].set<uint32_t>(face.mIndices[0]);
        indices[1][0].set<uint32_t>(face.mIndices[1]);
        indices[2][0].set<uint32_t>(face.mIndices[2]);

    }

    
    upload();

    models.push_back(model);

    return models.size()-1;
    
}