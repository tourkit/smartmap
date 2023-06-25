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

static char frag[128] = "test.frag";
static char boyfile[128] = "assets/media/bo.jpg";
            
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

Artnet artnet("2.0.0.222");

int main() {  

    uint8_t white[4] = { 255,255,0,255 };
    uint8_t nowhite[512];
    memset(&nowhite[0],0,512);

    float no[100];
    GL_PRINT(sizeof(float));

    Texture tex(boyfile);
    Texture tex2("C:/msys64/home/SysErr/old/smartmap/assets/media/smile.jpg");

    glEnable(GL_CLIP_DISTANCE0);
    glEnable(GL_CLIP_DISTANCE1);
    glEnable(GL_CLIP_DISTANCE2);
    glEnable(GL_CLIP_DISTANCE3);

    uint8_t min = 0, max = 255;

    int fileCheck1 = 0;
    int fileCheck2 = last_mil(quadfile);
    int perline = 32;

    while(true) window->render([&]() {

        fileCheck1 = last_mil(frag, [](){ ShaderProgram::pool[0]->reset(); }, fileCheck1);
        // fileCheck2 = last_mil(quadfile, [](){ VBO::pool[0]->reset(); }, fileCheck2);

        Draw2D(tex);

        artnet.run();



        // CLUSTER RENDER LOOP


        // // END OF LOOP
    
        gui->newframe();  
        ImGui::ShowDemoWindow();
        for (auto dmx : Artnet::data) {

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2,2));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));

            ImGui::Begin(std::string("Artnet Universe "+std::to_string(dmx.first)).c_str());

                int nwidth = (ImGui::GetWindowWidth()-12-(perline*2)) / perline;

                ImGui::SliderInt("perline" , &perline,1,64);
                
                for (int i = 0; i < 512; i++) {

                    ImGui::PushID(i);

                    ImGui::VSliderScalar("",  ImVec2(nwidth,40),    ImGuiDataType_U8, &dmx.second.chan[i],  &min,   &max,   "");

                    if ((i + 1) % perline != 0) ImGui::SameLine(0);

                    ImGui::PopID();

                }

            ImGui::End();

            ImGui::PopStyleVar(5);

            break;

        }
        ImGui::Begin("VIEW");
    // Draw lines
        for (int i = 0; i < Texture::pool.size(); i++) {

            
            ImGui::Image((void*)(intptr_t)(ImTextureID)(uintptr_t)Texture::pool[i]->id, ImVec2(Texture::pool[i]->width,Texture::pool[i]->height));
            ImGui::PushID(i+100);

            if (ImGui::Button("UPDATE")) VBO::pool[0]->update();
            ImGui::SameLine();
            if (ImGui::Button("RESET")) VBO::pool[0]->reset();
                   
            ImGui::PopID();
            ImGui::Separator();
        }


        ImGui::End();
        ImGui::Begin("KTRL");


            ImGui::SetWindowFontScale(1.5);
            if (ImGui::DragInt2("winsize", &WIDTH,1,0,1920)) window->setSize(WIDTH,HEIGHT);
            if (ImGui::DragInt2("winpos", &OFFX,1,0,2560)) window->setPos(OFFX,OFFY);
            ImGui::Separator();
            if (ImGui::InputText(" tex", boyfile, IM_ARRAYSIZE(boyfile))) {

                Texture::pool[0]->create(std::string(boyfile));


            }
            if (ImGui::InputText(" frag", frag, IM_ARRAYSIZE(frag))) {

                ShaderProgram::pool[0]->paths[1] =  "C:/msys64/home/SysErr/old/smartmap/assets/shader/"+std::string(frag);
                ShaderProgram::pool[0]->reset();


            }



        ImGui::End();

        gui->render();

 
    });

} 

