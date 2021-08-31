/*

    SMARTMAP v1 beta

                        */

#include "globals.hpp"

int WIDTH = 600, HEIGHT = 300;

int FW = 1920, FH = 12000;

GL::Window* window = new GL::Window(false,WIDTH,HEIGHT);

auto*  gui = new GUI{window->window}; 

GL::ShaderProgram* shader = nullptr;

void reloadShader() { 

    auto* temp = new GL::ShaderProgram(
        
        "C:/Users/ebkc/Documents/testmake/assets/shader/vertex_test.glsl", 
        "C:/Users/ebkc/Documents/testmake/assets/shader/fragment_test.glsl"
        
    );

    std::set<std::shared_ptr<GUI::Element>> deleted;
    for (auto e:gui->elements){ deleted.insert(e); e.get()->links.erase(shader);}
    
    delete shader;

    shader = temp;

    for (auto e:deleted) e.get()->links.insert(shader);

    shader->sendUniform("media", (int)1);
}  
 
VBO draw2D;
void Draw2D(const Texture& tex) {

    glBindTexture(GL_TEXTURE_2D, tex.id);
    shader->use();
    draw2D.draw();

}

int main() {


    draw2D.addQuad(999);
    
    VBO quad;
    quad.addQuad(0);
    quad.addQuad(1);

    gui->elements.insert(std::make_shared<GUI::Counter>("count"));
    gui->elements.insert(std::make_shared<GUI::SliderF>("feedback", 1, .9));
    gui->elements.insert(std::make_shared<GUI::SliderI>("texchoice", 1,0,0,15));
    gui->elements.insert(std::make_shared<GUI::SliderF>("blurv"));
    gui->elements.insert(std::make_shared<GUI::SliderF>("u_scale", 2,1));
    gui->elements.insert(std::make_shared<GUI::SliderF>("u_translate", 2,0,-1,1));
    auto quantity = gui->elements.insert(std::make_shared<GUI::SliderI>("quantity", 1,2,1,10)).first->get();
    
    reloadShader();

    Atlas atlas("assets/media/");
    atlas.link(*shader);

    // Image boy("assets/media/boy.jpg", true);
    // Texture boyTex(boy.width, boy.height, GL_RGB8, boy.i);
    // boyTex.bind(1);

    Texture passBuf(FW,FH, GL_RGB8);
    Texture outBuf(FW,FH, GL_RGBA8);
    FrameBuffer outFB(outBuf);
    FrameBuffer winFB(0);

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

