/*

    SMARTMAP v1 beta

                        */

#include "smartmap.hpp"

int WIDTH = 600, HEIGHT = 300;

int FW = 1920, FH = 1200;

GL::Window* window = new GL::Window(false,WIDTH,HEIGHT);

auto*  gui = new GUI{window->window}; 

GL::ShaderProgram* shader = nullptr;

void reloadShader() { 

    auto* temp = new GL::ShaderProgram(
        
        "C:/Users/ebkc/Documents/testmake/assets/shader/smartmap.vert", 
        "C:/Users/ebkc/Documents/testmake/assets/shader/smartmap.frag"
        
    );

    std::set<std::shared_ptr<GUI::Element>> deleted;
    for (auto e:gui->elements){ deleted.insert(e); e.get()->links.erase(shader);}
    
    delete shader;

    shader = temp;

    for (auto e:deleted) e.get()->links.insert(shader);

    shader->sendUniform("media", (int)1);
}  
 
Quad draw2D;
void Draw2D(const Texture& tex) {

    glBindTexture(GL_TEXTURE_2D, tex.id);
    shader->use();
    draw2D.draw();

}

int main() {

    
    VBO quad;
    quad.addQuad(1); // feedback
    quad.addQuad(2); // fixture

    gui->elements.insert(std::make_shared<GUI::Counter>("count"));
    gui->elements.insert(std::make_shared<GUI::SliderF>("feedback", 1, .9));
    gui->elements.insert(std::make_shared<GUI::SliderI>("texchoice", 1,0,0,15));
    gui->elements.insert(std::make_shared<GUI::SliderF>("blurv"));
    gui->elements.insert(std::make_shared<GUI::SliderF>("u_scale", 2,1));
    gui->elements.insert(std::make_shared<GUI::SliderF>("u_translate", 2,0,-1,1));
    auto quantity = gui->elements.insert(std::make_shared<GUI::SliderI>("quantity", 1,2,1,10)).first->get();
    
    reloadShader();

    Atlas atlas("assets/media/");
    atlas.link(shader);

    Texture passBuf(FW,FH, GL_RGB8);

    Texture outBuf(FW,FH, GL_RGB8);
    FrameBuffer outFB(outBuf);

    FrameBuffer winFB(0);

    RectF cells[10];

    for (int i = 0; i < 10; i++) {

        cells[i].size = {.1,1 }; 
        cells[i].pos.x = (i%10)*.1;
        // cells[i].pos.y = std::floor(i/10)*.1;

    }

    UBO cellsUBO(&cells[0], 10*sizeof(RectF), "cellsUBO"); 
    cellsUBO.link(shader);
    cellsUBO.send();

    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);

    float last = glfwGetTime();
    
    while(true) window->render([&]() {

        float now = glfwGetTime(); 

        // if (now - last > 2) { reloadShader(); last = now; }

        /* DRAW LOOP */
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

