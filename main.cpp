/*

    SMARTMAP v1 beta

                        */

#include "smartmap.hpp"

#define DEBUG

// ArtnetDevice artnet;

int WIDTH = 600, HEIGHT = 300;

int MAT_X = 5; 
int MAT_Y = 2;

auto* window = new GL::Window(false,WIDTH,HEIGHT,1920-WIDTH);

auto*  gui = new GUI{window->window}; 

// auto* shader = new ShaderProgram({"assets/shader/smartmap.vert", "assets/shader/smartmap.frag"});
// auto* shader = new ShaderProgram({"assets/shader/basic.vert", "assets/shader/basic.frag"});
 
Quad quad;

void Draw2D() {
// void Draw2D(const Texture& tex) {


    // glBindTexture(GL_TEXTURE_2D, tex.id);
    // shader->use();
    quad.draw();

}

int main() {  
    
    glEnable(GL_CLIP_DISTANCE0);
    glEnable(GL_CLIP_DISTANCE1);
    glEnable(GL_CLIP_DISTANCE2);
    glEnable(GL_CLIP_DISTANCE3);

    while(true) window->render([&]() {

        Draw2D();

        // artnet.run();


        // CLUSTER RENDER LOOP


        // // END OF LOOP
    
        gui->newframe();  
        ImGui::Begin("KTRL");
        ImGui::End();
        gui->render();

 
    });

} 

