#include "vbo.hpp"  
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

VBO::~VBO()  { destroy(); }

void VBO::destroy() {

    if (!vao) return;

    // VBO::pool.resize(0);

    glDisableVertexAttribArray(0); 
    glDisableVertexAttribArray(1); 
    glDisableVertexAttribArray(2); 
    glDisableVertexAttribArray(3); 

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &vao);

    vertices.resize(0);
    indices.resize(0);


}

void VBO::reset() {  import(path); }

void VBO::update() {

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,  vertices.size()*sizeof(Vertice) , &vertices[0], GL_STATIC_DRAW );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(Indice) , &indices[0], GL_STATIC_DRAW );

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, sizeof(Vertice), (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(Vertice), (GLvoid *) (2*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(Vertice), (GLvoid *) (4*sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_TRUE, sizeof(Vertice), (GLvoid *) (6*sizeof(float)));
    glEnableVertexAttribArray(3);

}

VBO::VBO(std::string path, int id) : path(path) , id(id) {  VBO::pool.push_back(this);   import(path); };

void VBO::import(std::string path) {    

    destroy();

    

    glGenBuffers(1, &vbo); glGenBuffers(1, &ibo); glGenVertexArrays(1, &vao);


    this->id = VBO::pool.size();
    

    float clip_x = -1, clip_y = 1;

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile("C:/msys64/home/SysErr/old/smartmap/assets/model/"+std::string(path), aiProcess_CalcTangentSpace       | 
		aiProcess_Triangulate            |
		aiProcess_JoinIdenticalVertices  |
		aiProcess_SortByPType);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) std::cout << "Failed to load OBJ file: " << importer.GetErrorString() << std::endl;

    auto mesh = scene->mMeshes[0];

    for (int i = 0; i < mesh->mNumVertices; i++) {

        const aiVector3D& vertex = mesh->mVertices[i];
        
        vertices.push_back({
            vertex.x,vertex.y, 
            mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y, 
            clip_x, clip_y, 
            this->id
        });
    
    }
    
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        
        const aiFace& face = mesh->mFaces[i];

        indices.push_back({(int)face.mIndices[0],(int)face.mIndices[1],(int)face.mIndices[2]});


    }


    update();



}


void VBO::draw(int count) {

    glBindVertexArray(vao); 

    glDrawElementsInstanced(GL_TRIANGLES, indices.size()*sizeof(Indice), GL_UNSIGNED_INT, 0, count);

}
	
