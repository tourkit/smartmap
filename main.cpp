/*

    SMARTMAP v1 beta

                        */

#include "smartmap.hpp"

int WIDTH = 600, HEIGHT = 300;

int FW = 1920, FH = 1200;

auto* window = new GL::Window(false,WIDTH,HEIGHT);

auto*  gui = new GUI{window->window}; 

auto* shader = new GL::ShaderProgram("assets/shader/smartmap.vert", "assets/shader/smartmap.frag");
 
Quad draw2D;
void Draw2D(const Texture& tex) {

    glBindTexture(GL_TEXTURE_2D, tex.id);
    shader->use();
    draw2D.draw();

}

int main() {

    gui->elements.insert(std::make_shared<GUI::Counter>("count"));
    gui->elements.insert(std::make_shared<GUI::SliderF>("feedback", 1, .9));
    gui->elements.insert(std::make_shared<GUI::SliderI>("texchoice", 1,0,0,15));
    gui->elements.insert(std::make_shared<GUI::SliderF>("blurv"));
    gui->elements.insert(std::make_shared<GUI::SliderF>("u_scale", 2,1));
    gui->elements.insert(std::make_shared<GUI::SliderF>("u_translate", 2,0,-1,1));
    auto quantity = gui->elements.insert(std::make_shared<GUI::SliderI>("quantity", 1,2,1,10)).first->get();

    Atlas atlas("assets/media/");
    atlas.link(shader);

    Texture passBuf(FW,FH, GL_RGB8);

    Texture outBuf(FW,FH, GL_RGB8);
    FrameBuffer outFB(outBuf);

    FrameBuffer winFB(0);

    auto mat = matrice(10,1);
    UBO cellsUBO(&mat[0], mat.size()*sizeof(RectF), "cellsUBO"); 
    cellsUBO.link(shader);
    cellsUBO.send();
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA); // OR glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    VBO quad;
    quad.addQuad(1); // feedback
    quad.addQuad(2); // fixture
    
    while(true) window->render([&]() {

        outFB.clear();

        passBuf.bind(); 
        shader->use();
        quad.draw(*quantity);

        passBuf.copy(outBuf);

        winFB.clear();
        Draw2D(outBuf);
    
        gui->draw();

        std::this_thread::sleep_for(std::chrono::microseconds(10));
 
    });

} 

