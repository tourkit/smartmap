/*

    SMARTMAP (beta)

                        */


#include "smartmap.hpp"

#define DEBUG

#include <windows.h>
#include <ctime>
#include <cstdint>

// ArtnetDevice artnet;

int WIDTH = 1920, HEIGHT = 1080;
int OFFX = 2560, OFFY = 0;

int MAT_X = 5; 
int MAT_Y = 2;

auto* window = new GL::Window(false,WIDTH,HEIGHT,OFFX);

auto*  gui = new GUI{window->window}; 

const char* frag = "C:\\msys64\\home\\SysErr\\old\\smartmap\\assets\\shader\\test.frag";

// auto* shader = new ShaderProgram({"assets/shader/smartmap.vert", "assets/shader/smartmap.frag"});
auto* shader = new ShaderProgram({"C:/msys64/home/SysErr/old/smartmap/assets/shader/basic.vert", "C:/msys64/home/SysErr/old/smartmap/assets/shader/test.frag"});

auto quadfile = "C:/msys64/home/SysErr/old/smartmap/assets/model/quad.obj";
VBO quad(quadfile);


void Draw2D(const Texture& tex) {

    glBindTexture(GL_TEXTURE_2D, tex.id);
    ShaderProgram::pool[0]->use();
    quad.draw();

}

int last_mil(const char* path, std::function<void()> cb = [](){}, int before = 0) {

    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    GetFileAttributesExA(path, GetFileExInfoStandard, &fileInfo);
    SYSTEMTIME st;
    FileTimeToSystemTime(&fileInfo.ftLastWriteTime, &st);

    auto now = st.wMilliseconds;
    if (before != now ) cb();
    return now;

}

int main() {  

    uint8_t white[4] = { 255,255,0,255 };
    uint8_t nowhite[512];
    memset(&nowhite[0],0,512);

    float no[100];
    GL_PRINT(sizeof(float));

    Texture tex((void*)&nowhite,4,4);
    
    tex.create("C:/msys64/home/SysErr/old/smartmap/assets/media/boy.jpg");

    glEnable(GL_CLIP_DISTANCE0);
    glEnable(GL_CLIP_DISTANCE1);
    glEnable(GL_CLIP_DISTANCE2);
    glEnable(GL_CLIP_DISTANCE3);

    uint8_t min = 0, max = 255;

    int fileCheck1 = 0;
    int fileCheck2 = last_mil(quadfile);
    
    while(true) window->render([&]() {

        last_mil(frag, [](){ ShaderProgram::pool[0]->reset(); }, fileCheck1);
        last_mil(quadfile, [](){ VBO::pool[0]->reset(); }, fileCheck2);

        Draw2D(tex);

        // artnet.run();


        // CLUSTER RENDER LOOP


        // // END OF LOOP
    
        gui->newframe();  
        ImGui::ShowDemoWindow();
        ImGui::Begin("VIEW");

        for (int i = 0; i < Texture::pool.size(); i++) {
        
            ImGui::Image((void*)(intptr_t)(ImTextureID)(uintptr_t)Texture::pool[0]->id, ImVec2(Texture::pool[0]->width,Texture::pool[0]->height));
        
        }


        ImGui::End();
        ImGui::Begin("KTRL");

        if (ImGui::DragInt2("winsize", &WIDTH,1,0,1920)) window->setSize(WIDTH,HEIGHT);
        if (ImGui::DragInt2("winpos", &OFFX,1,0,2560)) window->setPos(OFFX,OFFY);


        ImGui::End();
        ImGui::Begin("INSPECTOR");

        for (int i = 0; i < 32; i++) {

            if (i > 0) ImGui::SameLine();

            ImGui::PushID(i);

            ImGui::VSliderScalar("",  ImVec2(25,40),    ImGuiDataType_U8,     &nowhite[i],  &min,   &max,   "");

            ImGui::PopID();

        }

        ImGui::End();
        gui->render();

 
    });

} 

