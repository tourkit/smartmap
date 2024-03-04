#include "vbo.hpp"  

#include "model.hpp"  
#include "file.hpp"  
#include "entry.hpp"  
#include "log.hpp"  

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

VBO::VBO() {  

    vertices = addObj(vertices_s);    
    vertices->is_stride = false;

    indices = addObj(indices_s);
    indices->is_stride = false;

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

    for (auto m : models) delete m;

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

    // (s->size() * reserved);


    glBufferData(GL_ARRAY_BUFFER,  vertices->size()*vertices->reserved, vertices->data(), GL_STATIC_DRAW );
    // glBufferData(GL_ARRAY_BUFFER,  vertices->size(), &backup_quad[0], GL_STATIC_DRAW );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size(), indices->data(), GL_STATIC_DRAW );

    // make this parametric from Object vertices definition
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, vertices->s->size(), (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, vertices->s->size(), (GLvoid *) (2*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_TRUE, vertices->s->size(), (GLvoid *) (4*sizeof(float)));
    glEnableVertexAttribArray(3);

}

void VBO::draw(int count) {

    glBindVertexArray(vao); 

    glDrawElementsInstanced(GL_TRIANGLES, indices->size(), GL_UNSIGNED_INT, 0, count);

}
	

int VBO::import(File *file) {    

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

        v["ID"][0].set<uint32_t>(0);


    }
    
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        
        const aiFace& face = mesh->mFaces[i];

        auto indices = this->indices->push();

        if (i ==1 ) {
            
        // indices[0].set<uint16_t>(6868);
        // indices[1].set<uint16_t>(6868);
        // indices[2].set<uint16_t>(6868);

        }else{

            indices[0].set<uint16_t>(face.mIndices[0]);
            indices[1].set<uint16_t>(face.mIndices[1]);
            indices[2].set<uint16_t>(face.mIndices[2]);

        }

        logger.cout();
        auto &x = *this->indices->entrys[0];
        PLOGW<<"s; "<< this->indices->entrys.size();
        PLOGW<< x[0][0].get<uint16_t>();
        PLOGW<< x[1][0].get<uint16_t>();
        PLOGW<< x[2][0].get<uint16_t>();

    }

    //WTF 

    auto &x = *indices->entrys[0];

    PLOGW<< x[0][0].get<uint16_t>();
    PLOGW<< x[1][0].get<uint16_t>();
    PLOGW<< x[2][0].get<uint16_t>();

    upload();

    models.push_back(new Model(file, models.size()));

    return models.size()-1;
    
}