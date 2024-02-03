#include "vbo.hpp"  

#include "struct.hpp"  


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

VBO::VBO(std::string path, int id)  : id(id), path(path) {  

    static bool init = false;

    if (!init) {
        
        // vertices.name = "Vertex";  

        // vertices.add({

        //     "Position",
        //     "UV",
        //     "ID",
            
        // }); 
        
        // vertices.reserved = 4;

        // indices.name = "Index";  
        // indices.add({

        //     "Vertex",
        //     "Vertex",
        //     "Vertex"
            
        // });

        init = true;
    
    }
    
//     indices.reserved = 2;
    
//     // buffer.callback = [this](){ upload(); }; // should be on Struct

//     glGenBuffers(1, &vbo); glGenBuffers(1, &ibo); glGenVertexArrays(1, &vao);

//     import(path); 

//     upload();
    
}

// VBO::~VBO()  { 

//     glDisableVertexAttribArray(0); 
//     glDisableVertexAttribArray(1); 
//     glDisableVertexAttribArray(2); 
//     glDisableVertexAttribArray(3); 

//     glDeleteBuffers(1, &vbo);
//     glDeleteBuffers(1, &ibo);
//     glDeleteVertexArrays(1, &vao);

// }

// void VBO::upload() {

//     glBindVertexArray(vao);

//     glBindBuffer(GL_ARRAY_BUFFER, vbo);
//     glBufferData(GL_ARRAY_BUFFER,  vertices->reserved*vertices->byte_size , vertices->data(), GL_STATIC_DRAW );

//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->reserved*indices->byte_size , indices->data(), GL_STATIC_DRAW );

//     // make this parametric from Object vertices definition
//     glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, vertices->byte_size, (GLvoid *) 0);
//     glEnableVertexAttribArray(0);
//     glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, vertices->byte_size, (GLvoid *) (2*sizeof(float)));
//     glEnableVertexAttribArray(1);
//     glVertexAttribPointer(3, 1, GL_FLOAT, GL_TRUE, vertices->byte_size, (GLvoid *) (4*sizeof(float)));
//     glEnableVertexAttribArray(3);

// }

// void VBO::import(std::string path) {    

//     Assimp::Importer importer;

//     const aiScene* scene = importer.ReadFile("assets/model/"+std::string(path), aiProcess_CalcTangentSpace       | 
// 		aiProcess_Triangulate            |
// 		aiProcess_JoinIdenticalVertices  |
// 		aiProcess_SortByPType);

//     if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) PLOGW << "Failed to load OBJ file: " << importer.GetErrorString();

//     auto mesh = scene->mMeshes[0];

//     for (int i = 0; i < mesh->mNumVertices; i++) {

//         const aiVector3D& vertex = mesh->mVertices[i];

//         std::array<char,20> data;
//         memcpy(&data[0], &vertex.x, 8);
//         memcpy(&data[8], &mesh->mTextureCoords[0][i].x, 8);
//         memcpy(&data[16], &this->id, 4);

//         vertices->push(&data[0]);

//     }
    
//     for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        
//         const aiFace& face = mesh->mFaces[i];

//         indices->push(&face.mIndices[0]);

//     }

// }


// void VBO::draw(int count) {

//     glBindVertexArray(vao); 

//     glDrawElementsInstanced(GL_TRIANGLES, indices->reserved*indices->byte_size, GL_UNSIGNED_INT, 0, count);

// }
	
