/*

    SMARTMAP v1 beta

                        */

#include "smartmap.hpp"

int WIDTH = 300, HEIGHT = 150;

auto* window = new GL::Window(false,WIDTH,HEIGHT);

auto*  gui = new GUI{window->window}; 

ShaderProgram* shader = nullptr;

float count = 0;
void loadShader() { 
    if (shader) { delete shader; } 
    shader = new ShaderProgram({
        "../assets/shader/smartmap.vert", 
        "../assets/shader/smartmap.frag"
    }); 
    shader->sendUniform("count",count);

    count += .1;
}

FrameBuffer winFB(0);

Image boy("assets/media/boy.jpg", true);
Texture boyTex(boy.width,boy.height, GL_RGB8, boy.i);
int main() {

    VBO quad;
    quad.addQuad();

    while(true) window->render([&]() {

        loadShader();
        
        winFB.clear();
        boyTex.bind();
        shader->use();
        quad.draw(3);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

    });

} 

