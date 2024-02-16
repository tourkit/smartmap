

#pragma once

unsigned int width = 400, height = 200, pos_x = 0, pos_y = 0;
//unsigned int  width = 1920; height = 1080; pos_x = 2560; pos_y = 290;


#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <fstream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

void Boilerplate() {  

    // SET OPENGL

    GLuint width = 400, height = 300;
    GLuint pos_x = 0, pos_y = 0;

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

    std::ifstream fragFile("C:/users/root/cpp/smartmap/assets/shader/basic.frag");
    std::string fragCode((std::istreambuf_iterator<char>(fragFile)), (std::istreambuf_iterator<char>()));
    auto fragptr = (const GLchar* const ) fragCode.c_str();

    std::cout << fragCode;

    auto frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragptr, nullptr);
    glCompileShader(frag);
    // Check for errors

    GLchar infoLog[512];
    GLint success;

    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);

    if (!success) {

        glGetShaderInfoLog(frag, 512, NULL, infoLog);

        std::cout << infoLog;
        
    }


    std::ifstream vertFile("C:/users/root/cpp/smartmap/assets/shader/basic.vert");
    std::string vertCode((std::istreambuf_iterator<char>(vertFile)), (std::istreambuf_iterator<char>()));
    auto vertptr = (const GLchar* const ) vertCode.c_str();
    std::cout << vertCode;

    auto vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vertptr, nullptr);
    glCompileShader(vert);

    glGetShaderiv(vert, GL_COMPILE_STATUS, &success);

    if (!success) {

        glGetShaderInfoLog(vert, 512, NULL, infoLog);

        std::cout << infoLog;
        
    }


    glAttachShader(shader, frag);
    glAttachShader(shader, vert);

    glLinkProgram( shader );

    glUseProgram(shader); 

    std::vector<float> data = {.0,0,1,1};

    GLuint ubo;
    glGenBuffers(1, &ubo);

    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, data.size()*4, NULL, GL_DYNAMIC_COPY);


    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glUniformBlockBinding(shader, glGetUniformBlockIndex(shader, "ubo"), 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);


    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, data.size()*4, &data[0]); 

    // RENDER

    while (!glfwWindowShouldClose(window)) {

        lastTime = glfwGetTime();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // BG COLOR
        glClear(GL_COLOR_BUFFER_BIT); //|GL_STENCIL_BUFFER_BIT); ??

        glDrawElementsInstanced(GL_TRIANGLES, indices.size()*6, GL_UNSIGNED_INT, 0, 1);

        glfwPollEvents();

        glfwSwapBuffers(window);

    }

} 
