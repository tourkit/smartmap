#include "vbo.hpp"
#include "ubo.hpp"

#include "model.hpp"
#include "file.hpp"
#include "instance.hpp"
#include "engine.hpp"
#include "log.hpp"
#include "utils.hpp"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

VBO::VBO(File* file) : VBO() { add(file); }

VBO::VBO() : Buffer("VBO"), vertices("Vertices", 0), indices("Indices", 0) {

    vertices.add(vertice);
    Buffer::add(&vertices);

    indices.add(indice);
    Buffer::add(&indices);

    create();

}

void VBO::destroy() {

    init = false;

    for (int i = 0 ; i < enabled_attrs; i++) glDisableVertexAttribArray(i);

    glDeleteBuffers(1, &vbo);

    glDeleteBuffers(1, &ibo);

    glDeleteVertexArrays(1, &vao);

}

void VBO::create() {

    destroy();

    glGenBuffers(1, &vbo); glGenBuffers(1, &ibo);

    init = true;

}

VBO::~VBO()  { destroy(); }

void VBO::reset() {

    vertices.quantity(0);

    indices.quantity(0);

    upload();

}

void VBO::update() { Buffer::update(); }

void VBO::upload() {

    Instance inst(this);

    if (!inst[0].size() || !inst[1].size()) return;

    auto v_size = members[0]->footprint_all();

    auto i_size = members[1]->footprint_all();

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER,  v_size, data.data(), GL_STATIC_DRAW );

    if (!vao) {

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        int offset = 0;
        enabled_attrs = 0;
        for (auto & m : members[0]->members[0]->members) {

            auto type = GL_FLOAT;
            if (m->type() == typeid(int)) type = GL_INT;
            if (m->type() == typeid(uint32_t)) type = GL_UNSIGNED_INT;

            auto count = 1; // m->count(); does it
            if (m->type() == typeid(glm::vec2)) count = 2;
            else if (m->type() == typeid(glm::vec3)) count = 3;
            else if (m->type() == typeid(glm::vec4)) count = 4;

            glVertexAttribPointer(enabled_attrs, count, type, GL_TRUE, members[0]->footprint(), (const void*)(size_t)offset);

            glEnableVertexAttribArray(enabled_attrs++);

            offset+= m->footprint();

        }

    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, i_size, data.data() + v_size , GL_STATIC_DRAW );

    // std::string str;
    // str += "ID " + std::to_string(vbo) + " - ";
    // str += std::to_string(v_size) + " : ";
    // for (int i = 0 ; i < v_size; i++) { str += std::to_string(*(uint8_t*)(data.data()+i)) + " "; }
    // PLOGV << str;

}

void VBO::draw(int count) {

    glBindVertexArray(vao);

    glDrawElementsInstanced(GL_TRIANGLES, members[1]->footprint_all()/sizeof(int), GL_UNSIGNED_INT, 0, count);

}

void VBO::addQuad(float w, float h, float x, float y, int id) {

    int o = (*this)[0].def()->quantity();

    if (o) id = (*this)[0].eq(o-1)[0]["OBJ"].get<float>()+1;

    auto x_ = x;
    auto y_ = y;
    auto w_ = (x+w);
    auto h_ = (y+h);

    struct Vec { float a,b,c,d,e,f; float g,h; };

    (*this)[0].push()[0].set<Vec>({ x_*2-1 , y_*2-1 , 0, 1, x_ , y_ ,  (float)id, (float)layer_id});
    (*this)[0].push()[0].set<Vec>({ w_*2-1 , y_*2-1 , 1, 1, w_ , y_ ,  (float)id, (float)layer_id});
    (*this)[0].push()[0].set<Vec>({ x_*2-1 , h_*2-1 , 0, 0, x_ , h_ ,  (float)id, (float)layer_id});
    (*this)[0].push()[0].set<Vec>({ w_*2-1 , h_*2-1 , 1, 0, w_ , h_ ,  (float)id, (float)layer_id});

    (*this)[1].push()[0].set<std::array<int,3>>({o+0,o+1,o+2});
    (*this)[1].push()[0].set<std::array<int,3>>({o+1,o+2,o+3});

    upload();

}

bool VBO::add(File* file, int id) { if (add_noupload(file,id)) { upload(); return true; } return false; }

bool VBO::add_noupload(File* file, int id) {

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFileFromMemory(&file->data[0], file->data.size(), aiProcess_CalcTangentSpace       |
        aiProcess_Triangulate            |
        aiProcess_JoinIdenticalVertices  |
        aiProcess_SortByPType);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) { PLOGE << "Failed to load OBJ file: " << file->path_v << " . " << importer.GetErrorString(); return false; }

    auto mesh = scene->mMeshes[0];

    int next_indice =  members[0]->quantity();

    for (int i = 0; i < mesh->mNumVertices; i++) {

        const aiVector3D& vertex = mesh->mVertices[i];

        auto v = (*this)[0].push()[0];

        v["POSITION"].set<glm::vec2>({ vertex.x, vertex.y });
        v["UV"].set<glm::vec2>({ mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
        v["NORMALIZED"].set<glm::vec2>({ mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
        v["OBJ"].set<float>(id);
        v["LAYER"].set<float>(layer_id);

    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {

        const aiFace& face = mesh->mFaces[i];
        auto indice = (*this)[1].push()[0];

        indice[0].set<int>(next_indice+face.mIndices[0]);
        indice[1].set<int>(next_indice+face.mIndices[1]);
        indice[2].set<int>(next_indice+face.mIndices[2]);

    }

    return true;

}
