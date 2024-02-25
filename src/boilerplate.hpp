

#pragma once

unsigned int width = 400, height = 200, pos_x = 0, pos_y = 0;
//unsigned int  width = 1920; height = 1080; pos_x = 2560; pos_y = 290;


#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>


#include "image.hpp"


#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

struct Boilerplate {

Boilerplate() {  

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


    Image img2("assets/media/boy.jpg");
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
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,img2.width, img2.height,GL_RGB,GL_UNSIGNED_BYTE,&img2.data[0]);
    glGenerateMipmap(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0); 

    GLuint textureUniformLoc = glGetUniformLocation(shader, "texture0");
    glUniform1i(textureUniformLoc, 1);

   // GUI

    ImGui::CreateContext(); 
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");


    // RENDER

    while (!glfwWindowShouldClose(window)) {

        lastTime = glfwGetTime();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // BG COLOR
        glClear(GL_COLOR_BUFFER_BIT); //|GL_STENCIL_BUFFER_BIT); ??

        glDrawElementsInstanced(GL_TRIANGLES, indices.size()*6, GL_UNSIGNED_INT, 0, 1);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("yellop");

        float ratio = img2.height/(float)img2.width;
        auto nw = std::min(img2.width,512);

        ImGui::Image(&tex2, ImVec2(nw,nw*ratio));

        ImGui::ShowDemoWindow();

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwPollEvents();

        glfwSwapBuffers(window);

    }

} 
};

static Boilerplate boiler;