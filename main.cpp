#include "globals.hpp"

int WIDTH = 600, HEIGHT = 300;

GL::Window* window = new GL::Window(false,WIDTH,HEIGHT);

auto*  gui = new GUI{window->window}; 

GL::ShaderProgram* shader = nullptr;

void reloadShader() { 

    auto* temp = new GL::ShaderProgram(
        
        "C:/Users/ebkc/Documents/testmake/assets/shader/vertex_test.glsl", 
        "C:/Users/ebkc/Documents/testmake/assets/shader/fragment_test.glsl"
        
    );

    for (auto e:gui->elements) e.get()->links.erase(shader);
    
    delete shader;

    shader = temp;

    for (auto e:gui->elements) e.get()->links.insert(shader);


}  

 
int main() {

    
    reloadShader();

    gui->elements.insert(std::make_shared<GUI::Counter>("count"));
    gui->elements.insert(std::make_shared<GUI::SliderF>("feedback", 1, .9));
    gui->elements.insert(std::make_shared<GUI::SliderI>("texchoice", 1,0,0,15));
    gui->elements.insert(std::make_shared<GUI::SliderF>("blurv"));
    gui->elements.insert(std::make_shared<GUI::SliderF>("u_scale", 2,1));
    gui->elements.insert(std::make_shared<GUI::SliderF>("u_translate", 2,0,-1,1));

    VBO quad;
    quad.addQuad(0);
    quad.addQuad(1);

    // window->clickCallBack = [&]() { reloadShader(); };

    Image boy("C:/Users/ebkc/Documents/testmake/assets/media/boy.jpg", true);
    Texture boyTex(boy.width, boy.height, GL_RGB8, boy.i);

    FrameBuffer winFB(0);

    float last= glfwGetTime();
    
    while(true) window->render([&]() {

        float now = glfwGetTime(); 

        if (now - last > 2) { reloadShader(); last = now; }

        winFB.clear();

        boyTex.bind();

        shader->use();

        quad.draw(2);
    
        gui->draw();

        std::this_thread::sleep_for(std::chrono::microseconds(10));
 
    });

} 


