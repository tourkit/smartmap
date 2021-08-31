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
void Draw2D(GLuint tex) {

    glBindTexture(GL_TEXTURE_2D, tex);
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
    
    reloadShader();

    Image boy("C:/Users/ebkc/Documents/testmake/assets/media/boy.jpg", true);
    Texture boyTex(boy.width, boy.height, GL_RGB8, boy.i);
    boyTex.bind(1);

    Texture passBuf(FW,FH, GL_RGB8);
    Texture outBuf(FW,FH, GL_RGBA8);
    FrameBuffer fb(outBuf);
    FrameBuffer winFB(0);

    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);

    float last = glfwGetTime();
    
    while(true) window->render([&]() {

        float now = glfwGetTime(); 

        if (now - last > 2) { reloadShader(); last = now; }

        /* DRAW LOOP */
        fb.clear();

        passBuf.bind(); 
        shader->use();
        quad.draw(2);

        passBuf.copy(outBuf);

        winFB.clear();
        Draw2D(outBuf);
    
        gui->draw();

        std::this_thread::sleep_for(std::chrono::microseconds(10));
 
    });

} 


