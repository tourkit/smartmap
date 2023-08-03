/*

    SMARTMAP (beta)

                        */


#pragma once

unsigned int width = 400, height = 200, pos_x = 2560-400, pos_y = 0;
//unsigned int  width = 1920; height = 1080; pos_x = 2560; pos_y = 290;

#define BOILNOT
#ifdef BOIL

#include <chrono>
#include <thread>
#include <vector>
#include <fstream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "src/file.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

int Boilerplate() {  

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

    std::ifstream fragFile("shader/basic.frag");
    std::string fragCode((std::istreambuf_iterator<char>(fragFile)), (std::istreambuf_iterator<char>()));
    auto fragptr = (const GLchar* const ) fragCode.c_str();

    auto frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragptr, nullptr);
    glCompileShader(frag);

    std::ifstream vertFile("shader/basic.vert");
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

    GLuint tex;

    struct FT { 

        FT_Library library;
        FT_Face face;
        FT_GlyphSlot slot;

        GLuint width;
        GLuint height;
        
        void* buffer;

        FT() {
            
            FT_Init_FreeType(&library);
            
            FT_New_Face(library, "Anonymous.ttf", 0, &face); 

            FT_Set_Pixel_Sizes(face, 0, 200);
            
            slot = face->glyph;
            FT_Load_Char(face, 'A', FT_LOAD_RENDER);

            buffer = slot->bitmap.buffer;
            width = slot->bitmap.width;
            height = slot->bitmap.rows;

        }

        ~FT() {
           
            FT_Done_Face(face);
            FT_Done_FreeType(library);

        }

    } img;


    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, img.width, img.height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,img.width, img.height,GL_RED,GL_UNSIGNED_BYTE,img.buffer);
    glGenerateMipmap(GL_TEXTURE_2D);

    Image img2("container.jpg");
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

    GLuint textureUniformLoc = glGetUniformLocation(shader, "mediaAtlas");
    glUniform1i(textureUniformLoc, 1);

    // RENDER

    

    while (!glfwWindowShouldClose(window)) {

        if (glfwGetTime() - lastTime <= 1./280. ) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }
    
        lastTime = glfwGetTime();

        glClearColor(0.5f, 0.0f, 0.0f, 1.0f); // BG COLOR
        glClear(GL_COLOR_BUFFER_BIT); //|GL_STENCIL_BUFFER_BIT); ??

        glDrawElementsInstanced(GL_TRIANGLES, indices.size()*6, GL_UNSIGNED_INT, 0, 1);

        glfwPollEvents();

        glfwSwapBuffers(window);

    }




} 

#else

#include "artnet.hpp"

#include <ctime>
#include <cstdint>
#include <map>
#include <functional>

static inline std::map<int,int> filechecks;
static inline int survey_count = 0;
static inline void survey(const char* path, std::function<void()> cb = [](){}) {

    WIN32_FILE_ATTRIBUTE_DATA fileInfo; GetFileAttributesExA(path, GetFileExInfoStandard, &fileInfo);
    SYSTEMTIME st; FileTimeToSystemTime(&fileInfo.ftLastWriteTime, &st);
    auto last = st.wMilliseconds;

    if (filechecks[survey_count] != last) { filechecks[survey_count] = last;  cb(); }
    survey_count++;

}


#include "window.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "vbo.hpp"
#include "framebuffer.hpp"
#include "atlas.hpp"
#include "ubo.hpp"
#include "gui.hpp"
#include "artnet.hpp"
#include "ndi.hpp"
#include "citp.hpp"

#include "imgui/imgui.h"

void Boilerplate() {  


    GLuint width = 400, height = 200;
    // GLuint pos_x = 1920-400, pos_y = 0;  

    // Artnet artnet("2.0.0.222");

    auto lastTime = glfwGetTime();

    Window window(false,width,height,pos_x,pos_y);
    
    GUI gui(window.window);

    VBO quad0("quad.obj",0, width,height);

    Texture img("boy.jpg");

    ShaderProgram shader({"basic.vert", "basic.frag"});

    std::vector<float> debuguniforms{0,1,.5,0,0,.15,0,0,0,0,0,0,0,0,0,0};

    NDI::Sender ndi(width,height);

    CITP citp(width,height);

    

    // char buffer[1024];
    // socket.receive(buffer, 1024);


    FrameBuffer fb(0,width,height);

    while (!glfwWindowShouldClose(window.window)) {

        fb.read(width,height,0,0,GL_RGBA,ndi.NDI_video_frame->p_data);

        ndi.send();

        citp.send();

        if (glfwGetTime() - lastTime <= 1./60 ) { std::this_thread::sleep_for(std::chrono::milliseconds(1)); continue; }
        lastTime = glfwGetTime();
        
        // artnet.run();    
        
        // frr.addChar((chars+(int)(debuguniforms[0]*61)),100);

        for (int i = 0; i < 10; i++) shader.sendUniform("debug"+std::to_string(i), debuguniforms[i]);

        survey_count = 0;
        survey(("C:/msys64/home/SysErr/old/smartmap/assets/shader/"+std::string(shader.paths[0])).c_str(), [&](){ shader.reset();  shader.use(); });
        survey(("C:/msys64/home/SysErr/old/smartmap/assets/shader/"+std::string(shader.paths[1])).c_str(), [&](){ shader.reset();  shader.use(); });

        glfwPollEvents(); 

        glClearColor(0.1f, 0.0f, 0.0f, 0.0f); // BG COLOR
        glClear(GL_COLOR_BUFFER_BIT); //|GL_STENCIL_BUFFER_BIT); ??

        quad0.draw();

        gui.newframe();

        ImGui::Begin("test");

        for (int i = 0; i < debuguniforms.size(); i++) ImGui::SliderFloat(("debug "+std::to_string(i)).c_str(), &debuguniforms[i], 0,1); 

        ImGui::End();

        gui.render();

        glfwSwapBuffers(window.window);

    }

} 


#endif
