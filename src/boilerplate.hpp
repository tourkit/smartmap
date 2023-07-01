/*

    SMARTMAP (beta)

                        */


#pragma once

unsigned int width = 400, height = 300, pos_x = 2560-width, pos_y = 0;
//unsigned int  width = 1920; height = 1080; pos_x = 2560; pos_y = 290;

#define BOILq
#ifdef BOIL

#include <chrono>
#include <thread>
#include <vector>
#include <fstream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "src/file.hpp"


int Boilerplate() {  

    // SET OPENGL

    GLuint width = 400, height = 300;
    auto lastTime = glfwGetTime();
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // SET WINDOW

    auto window = glfwCreateWindow(width, height, "OUTPUT", nullptr, nullptr);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    gl3wInit();
  
    // SET QUAD

    std::vector<std::array<float, 4>> vertices;
    vertices.push_back({-1, -1, 0, 1});
    vertices.push_back({1, -1, 1, 1});
    vertices.push_back({-1, 1, 0, 0});
    vertices.push_back({1, 1, 1, 0});


    std::vector<std::array<int, 3>> indices;
    indices.push_back({0,1,2});
    indices.push_back({1,2,3});

    GLuint vao,vbo,ibo;

    glGenBuffers(1, &vbo); glGenBuffers(1, &ibo); glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,  vertices.size()*16 , &vertices[0], GL_STATIC_DRAW );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*12 , &indices[0], GL_STATIC_DRAW );

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, 16, (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 16, (GLvoid *) (8));
    glEnableVertexAttribArray(1);

    // SET SHADER

    auto shader = glCreateProgram();

    // std::ifstream fragFile("assets/shader/basic.frag");
    std::ifstream fragFile("test.frag");
    std::string fragCode((std::istreambuf_iterator<char>(fragFile)), (std::istreambuf_iterator<char>()));
    auto fragptr = (const GLchar* const ) fragCode.c_str();

    auto frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragptr, nullptr);
    glCompileShader(frag);

    std::ifstream vertFile("basic.vert");
    // std::ifstream vertFile("assets/shader/basic.vert");
    std::string vertCode((std::istreambuf_iterator<char>(vertFile)), (std::istreambuf_iterator<char>()));
    auto vertptr = (const GLchar* const ) vertCode.c_str();

    auto vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vertptr, nullptr);
    glCompileShader(vert);


    glAttachShader(shader, frag);
    glAttachShader(shader, vert);

    glLinkProgram( shader );

    glUseProgram(shader); 

    // SET TEXTURE

    Image img("C:/msys64/home/SysErr/old/smartmap/assets/media/boy.jpg");


    GLuint tex;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, img.width, img.height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glBindTexture(GL_TEXTURE_2D, tex);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,img.width, img.height,GL_RGB,GL_UNSIGNED_BYTE,img.i);

    glGenerateMipmap(GL_TEXTURE_2D);

    Image img2("C:/msys64/home/SysErr/old/smartmap/assets/media/container.jpg");
    GLuint tex2;

    glGenTextures(1, &tex2);
    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, tex2);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, img2.width, img2.height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glActiveTexture(GL_TEXTURE0); 


    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, tex2);
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,img2.width, img2.height,GL_RGB,GL_UNSIGNED_BYTE,img2.i);
    glGenerateMipmap(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0); 

    GLuint textureUniformLoc = glGetUniformLocation(shader, "mediaAtlas");
    glUniform1i(textureUniformLoc, 1);

    // RENDER

    

    while (true) {

        if (glfwGetTime() - lastTime <= 1./280. ) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }
    
        lastTime = glfwGetTime();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // BG COLOR
        glClear(GL_COLOR_BUFFER_BIT); //|GL_STENCIL_BUFFER_BIT); ??


        glBindVertexArray(vao); 

        glDrawElementsInstanced(GL_TRIANGLES, indices.size()*12, GL_UNSIGNED_INT, 0, 1);

        glfwPollEvents();

        glfwSwapBuffers(window);

    }


} 

#else

#include "smartmap.hpp"

int Boilerplate() {  
    
    auto lastTime = glfwGetTime();

    GL::Window window(false,width,height,pos_x,pos_y);
    
    VBO quad;

    ShaderProgram shader({"basic.vert", "test.frag"});
    
    Texture tex("C:/msys64/home/SysErr/old/smartmap/assets/media/boy.jpg");

    Atlas atlas("assets/media/");
    atlas.link(&shader);

    while (true) {

        if (glfwGetTime() - lastTime <= 1./280. ) { std::this_thread::sleep_for(std::chrono::milliseconds(1)); continue; }

        lastTime = glfwGetTime();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // BG COLOR
        glClear(GL_COLOR_BUFFER_BIT); //|GL_STENCIL_BUFFER_BIT); ??

        quad.draw();

        glfwPollEvents();

        glfwSwapBuffers(window.window);

    }

} 


#endif
