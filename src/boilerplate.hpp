

#pragma once

// static unsigned int width = 400;
// static unsigned int height = 200;
// static unsigned int pos_x = 0;
// static unsigned int pos_y = 0;


#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>


#include "image.hpp"
#include "log.hpp"


#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

struct BoilerQuad {

    GLuint vao = 0,vbo = 0,ibo = 0;

    static inline std::vector<std::array<float, 4>> vertices = { {-.5, -.5, 0, 1}, {.5, -.5, 1, 1}, {-.5, .5, 0, 0}, {.5, .5, 1, 0} };

    static inline std::vector<std::array<int, 3>> indices ={ {0,1,2}, {1,2,3} };

    ~BoilerQuad() {

        glBindVertexArray(vao);
        glDisableVertexAttribArray(0);
        glDeleteVertexArrays(1, &vao);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ibo);
        glDisableVertexAttribArray(1);

        vbo = 0;
        ibo = 0;
        vao = 0;

    }

    BoilerQuad() {

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(std::array<float, 4>), vertices.data(), GL_STATIC_DRAW);

        if (!vao) {

            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
            glEnableVertexAttribArray(1);

        }

        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(std::array<int, 3>), indices.data(), GL_STATIC_DRAW);

    }

    void draw() {

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        glDrawElementsInstanced(GL_TRIANGLES, indices.size()*3, GL_UNSIGNED_INT, 0, 1);

    }

};


struct BoilerTexture {

    Image image;
    GLuint texture;

    BoilerTexture(std::string file = "assets/medias/boy.jpg") : image(file) {

        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0+1);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, image.width, image.height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glActiveTexture(GL_TEXTURE0);


        glActiveTexture(GL_TEXTURE0+1);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexSubImage2D(GL_TEXTURE_2D,0,0,0,image.width, image.height,GL_RGB,GL_UNSIGNED_BYTE,&image.data[0]);
        glGenerateMipmap(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);


    }
};

struct BoilerShader {

    GLuint id = 0, frag_id = 0, vert_id = 0;

    std::string frag = "#version 430 core \nstruct Ubo { int x,y,z,w;};\nlayout (binding = 0, std140) uniform ubo_  { Ubo ubo;  };\n\nout vec4 COLOR;\nin vec2 UV;\nvoid main() { COLOR = vec4(1,UV.x,float(ubo.z)/10.0f,1); }";
    
    std::string vert = "#version 430 core\nstruct Ubo { int x,y,z,w;};\nlayout (binding = 0, std140) uniform ubo_  { Ubo ubo;  };\n\nlayout (location = 0) in vec2 POSITION;\nlayout (location = 1) in vec2 TEXCOORD;\nout vec2 UV;\nvoid main() { UV = TEXCOORD; gl_Position = vec4(POSITION.x,POSITION.y,0,1); }";
    
    BoilerShader() { create(); }

    void destroy() {

        if (id) glDeleteProgram(id);

        if (frag_id) glDeleteShader(frag_id);

        if (vert_id) glDeleteShader(vert_id);

    }

    void create() {

        destroy();

        id = glCreateProgram();

        auto fragptr = (const GLchar* const ) frag.c_str();

        frag_id = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(frag_id, 1, &fragptr, nullptr);
        glCompileShader(frag_id);
        // Check for errors

        GLchar infoLog[512];
        GLint success;

        glGetShaderiv(frag_id, GL_COMPILE_STATUS, &success);

        if (!success) {

            glGetShaderInfoLog(frag_id, 512, NULL, infoLog);

            PLOGW << infoLog;

        }

        auto vertptr = (const GLchar* const ) vert.c_str();

        vert_id = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vert_id, 1, &vertptr, nullptr);
        glCompileShader(vert_id);

        glGetShaderiv(vert_id, GL_COMPILE_STATUS, &success);

        if (!success) {

            glGetShaderInfoLog(vert_id, 512, NULL, infoLog);

            PLOGW << infoLog;

        }


        glAttachShader(id, frag_id);
        glAttachShader(id, vert_id);

        glLinkProgram( id );

        glUseProgram(id);

    }

    void use() { glUseProgram(id); }

    void texture(std::string name = "tex", int loc = 0){ auto x = glGetUniformLocation(id, name.c_str()) ; glUniform1i(x, loc); }

};


struct BoilerUBO {

    std::vector<int> data = {1,2,10,100};

    int binding = 0;
    std::string name = "ubo";

    GLuint id;

    BoilerUBO(GLuint shader) {

        glGenBuffers(1, &id);

        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferData(GL_UNIFORM_BUFFER, data.size()*4, NULL, GL_DYNAMIC_COPY);

        bind(shader);

        upload();
    }

    void upload(){ 

        std::string str;
        for (int i = 0 ; i < data.size()*sizeof(int); i++) str+= std::to_string(*(((uint8_t*)data.data())+i)) + " ";
        PLOGW << name << " " << id << " " << binding << ": " << data.size()*4 << " - " << str;

        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, data.size()*4, &data[0]);

    }

    void bind(GLuint shader){ 
       
        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glUniformBlockBinding(shader, glGetUniformBlockIndex(shader, name.c_str()), binding);
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, id);

    }

};

struct BoilerWindow {

    GLuint width = 400, height = 300;
    GLuint pos_x = 0, pos_y = 0;

    double lastTime = glfwGetTime();

    GLFWwindow* window;

    BoilerWindow() {

        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(width, height, "OUTPUT", nullptr, nullptr);

        glfwMakeContextCurrent(window);
        glfwSwapInterval(0);

        gl3wInit();

    }

    void run(std::function<void()> cb) {

            while (!glfwWindowShouldClose(window)) {

                lastTime = glfwGetTime();


                glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // BG COLOR
                glClear(GL_COLOR_BUFFER_BIT); //|GL_STENCIL_BUFFER_BIT); ??

                cb();

                glfwPollEvents();

                glfwSwapBuffers(window);

            }
    }

};

struct BoilerGUI {

    BoilerGUI(BoilerWindow& window) {

        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window.window, true);
        ImGui_ImplOpenGL3_Init("#version 430");


    }
};
#include "editor.hpp"
#include "engine.hpp"

struct Boilerplate {

    static void Editors() {

        Editor<BoilerShader>([](Node* node, BoilerShader *shader){

         ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) {

            if (ImGui::BeginTabItem("fragment")) {

                static TextEditor frageditor;

                static bool init = false;
                if (!init){

                    frageditor.SetShowWhitespaces(false);
                    frageditor.SetReadOnly(false);
                    frageditor.SetText(shader->frag.c_str());

                    init = true;
                }

                frageditor.Render("frageditor");

                if (frageditor.IsTextChanged()) {

                    auto x = frageditor.GetText();

                    memset(&x[frageditor.GetText().length()],0,1);

                    shader->frag  = x;

                    shader->create();


                }

                ImGui::EndTabItem();

            }

            if (ImGui::BeginTabItem("vertex")) {

                static TextEditor verteditor;

                static bool init = false;
                if (!init){

                    verteditor.SetShowWhitespaces(false);
                    verteditor.SetReadOnly(false);
                    verteditor.SetText(shader->vert.c_str());

                    init = true;
                }

                verteditor.Render("frageditor");

                if (verteditor.IsTextChanged()) {

                    auto x = verteditor.GetText();

                    memset(&x[verteditor.GetText().length()],0,1);

                    shader->vert  = x;

                    shader->create();


                }

                ImGui::EndTabItem();

            }

            ImGui::EndTabBar();

        }

    });

    }

    static void Init(bool run = true) {

        BoilerWindow window;

        {BoilerQuad quadq;}
        BoilerQuad quad;

        BoilerShader shader;

        // BoilerUBO ubo(shader);

        // BoilerTexture texture;
        // shader.texture();

        // BoilerGUI gui(window);

        // RENDER

        if (run)

            window.run([&](){

                quad.draw();

                // ImGui_ImplOpenGL3_NewFrame();
                // ImGui_ImplGlfw_NewFrame();
                // ImGui::NewFrame();

                // ImGui::Begin("yellop");

                // float ratio = texture.image.height/(float)texture.image.width;
                // auto nw = std::min(texture.image.width,512);

                // ImGui::Image(&texture.texture, ImVec2(nw,nw*ratio));

                // ImGui::ShowDemoWindow();

                // ImGui::End();

                // ImGui::Render();
                // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            });

    }
};
