#include "vbo.hpp"  

#include "engine.hpp"  
#include "file.hpp"  

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

VBO::VBO() {  

    static bool init = false;

    if (!init) { 

        PLOGD << "init";
        
        vertices = Engine::getInstance().dynamic_ubo->buffer.addObj(new Struct("Vertex", {"Position","UV","ID",}));

        indices = Engine::getInstance().dynamic_ubo->buffer.addObj(new Struct("Index",{"Vertex", "Vertex", "Vertex"}));

        // vertices->push();

        init = true;
    
    }

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

void VBO::upload() {

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,  vertices->size(), vertices->data(), GL_STATIC_DRAW );

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
	

void VBO::import(File *file) {    

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFileFromMemory(&file->data[0], file->data.size(), aiProcess_CalcTangentSpace       | 
        aiProcess_Triangulate            |
        aiProcess_JoinIdenticalVertices  |
        aiProcess_SortByPType);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) PLOGW << "Failed to load OBJ file: " << importer.GetErrorString();

    auto mesh = scene->mMeshes[0];

    for (int i = 0; i < mesh->mNumVertices; i++) {

        const aiVector3D& vertex = mesh->mVertices[i];


        std::array<float,2> v = {vertex.x, vertex.y};
        // vertices->push(&v);
        // vertices->push();

        
        // uvs.push_back({mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y});
    }
    
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        
        const aiFace& face = mesh->mFaces[i];

        // indices.push_back({face.mIndices[0], face.mIndices[1], face.mIndices[2]});

    }

}