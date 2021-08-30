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

    gui->elements.insert(std::make_shared<GUI::Counter>("count"));
    gui->elements.insert(std::make_shared<GUI::SliderF>("feedback"));
    gui->elements.insert(std::make_shared<GUI::SliderF>("blurv"));
    gui->elements.insert(std::make_shared<GUI::SliderF>("u_scale", 2,1));
    gui->elements.insert(std::make_shared<GUI::SliderF>("u_translate", 2,0,-1,1));

    

    // float sss[2] = {1,1};//.5,.5};
    // float ppp[2] = {0,0};
    // float feedback = 0.95;
    // float blurv = 0;
    // int texchoice = 0;
    // float count = 0;



    VBO quad;
    quad.addQuad(0);
    quad.addQuad(1);

    reloadShader();

    window->clickCallBack = [&]() { reloadShader(); };

    Image boy("C:/Users/ebkc/Documents/testmake/assets/media/boy.jpg", true);
    Texture boyTex(boy.width, boy.height, GL_RGB8, boy.i);

    float last= glfwGetTime();
    
    while(true) window->render([&]() {

        float now = glfwGetTime(); 

        if (now - last > 1) { reloadShader(); last = now; }

        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);

        boyTex.bind();
        shader->use();

        quad.draw(10);
    
        gui->draw();

        std::this_thread::sleep_for(std::chrono::microseconds(10));
 
    });

} 


