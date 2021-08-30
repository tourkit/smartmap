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

GL::Window* window = new GL::Window(false,200,100);

GL::ShaderProgram* shader = nullptr;

void reloadShader() { 

    delete shader;

    shader = new GL::ShaderProgram(
        
        "C:/Users/ebkc/Documents/testmake/assets/shader/vertex_test.glsl", 
        "C:/Users/ebkc/Documents/testmake/assets/shader/fragment_test.glsl"
        
    );

 }

int main() {

    std::vector<float> vertices =
    {
         0, 1,     1, 1,

         0, 0,     1, 0
    };

    std::vector<int> indices = { 0,1,2, 1,2,3 };

    GLuint vao, vbo,ibo;
    glGenBuffers(1, &vbo); glGenBuffers(1, &ibo); glGenVertexArrays(1, &vao); 

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,  vertices.size()*sizeof(float) , &vertices[0], GL_STATIC_DRAW );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(int) , &indices[0], GL_STATIC_DRAW );

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, 2*sizeof(float), (GLvoid *) 0);
    glEnableVertexAttribArray(0);

    reloadShader();

    window->clickCallBack = [&]() { reloadShader(); };

    float last= glfwGetTime();
    float time = .2; 

    
    while(true) window->render([&]() {

        float now = glfwGetTime(); 
        float delta = now - last;

        if (delta > time) {

            reloadShader();
            last = now;

        }
    
        shader->sendUniform("x", (float)(std::sin(now)*.5+.5));

        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);

        shader->use();
    
        glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, 2);

        std::this_thread::sleep_for(std::chrono::microseconds(10));


    });

    glDisableVertexAttribArray(0);

} 


