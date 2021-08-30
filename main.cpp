#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <cmath>

#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <cmath>

#include "window.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "framebuffer.hpp"

#include "file.hpp"

int WIDTH = 200, HEIGHT = 100;

GL::Window* window = new GL::Window(false,WIDTH,HEIGHT);

GL::ShaderProgram* shader = nullptr;

void reloadShader() { 


    auto* temp = new GL::ShaderProgram(
        
        "C:/Users/ebkc/Documents/testmake/assets/shader/vertex_test.glsl", 
        "C:/Users/ebkc/Documents/testmake/assets/shader/fragment_test.glsl"
        
    );

    delete shader;

    shader = temp;

}

struct Quad {


       struct Vertice { float x,y; int id; }; 
    struct Indice { int a,b,c; }; 



    std::vector<Vertice> vertices;

    std::vector<Indice> indices;

    GLuint vao, vbo,ibo;

    ~Quad() { glDisableVertexAttribArray(0); } // disable at delete or en of constr ?

    Quad() {    

        glGenBuffers(1, &vbo); glGenBuffers(1, &ibo); glGenVertexArrays(1, &vao); 

        addQuad();

    }    
    
    void upload() {

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER,  vertices.size()*sizeof(Vertice) , &vertices[0], GL_STATIC_DRAW );

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(Indice) , &indices[0], GL_STATIC_DRAW );

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, sizeof(Vertice), (GLvoid *) 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_TRUE, sizeof(Vertice), (GLvoid *) (2*sizeof(float)));
        glEnableVertexAttribArray(1);

    }

    void addQuad(int id = 0) {

        int pos = indices.size()*sizeof(Indice);

        vertices.push_back({0, 1, id});
        vertices.push_back({1, 1, id});
        vertices.push_back({0, 0, id});
        vertices.push_back({1, 0, id});

        indices.push_back({pos+0,pos+1,pos+2});
        indices.push_back({pos+1,pos+2,pos+3});

        upload();

    }

    void draw(int count = 1) {

        glBindVertexArray(vao); 

        glDrawElementsInstanced(GL_TRIANGLES, indices.size()*sizeof(Indice), GL_UNSIGNED_INT, 0, count);

    }

} quad;

int main() {

    quad.addQuad(1);
    reloadShader();

    window->clickCallBack = [&]() { reloadShader(); };

    Image boy("C:/Users/ebkc/Documents/testmake/assets/media/boy.jpg", true);
    Texture boyTex(boy.width, boy.height, GL_RGB8, boy.i);

    float last= glfwGetTime();
    
    while(true) window->render([&]() {

        float now = glfwGetTime(); 

        if (now - last > 1) { reloadShader(); last = now; }
    
        // shader->sendUniform("x", (float)(std::sin(now)*.5+.5));

        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);

        boyTex.bind();
        shader->use();

        quad.draw(10);
      
        std::this_thread::sleep_for(std::chrono::microseconds(10));
 
    });

} 


