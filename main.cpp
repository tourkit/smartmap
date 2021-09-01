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


struct FixtureUBO {

    vec2 size{1,1}, pos{0,0};

    vec4 rgba;

    vec4 gobo;

    float feedback;
    float strobe;

    vec2 useless; // pack size for std140

} fixtures[10];


int main() {

    // std::cout << f.gobo <<std::endl;

    auto quantity = gui->elements.insert(std::make_shared<GUI::SliderI>("quantity", 1,2,1,10)).first->get();
    
    gui->elements.insert(std::make_shared<GUI::Counter>("count"));
    gui->elements.insert(std::make_shared<GUI::SliderF>("feedback", 1, .9));
    gui->elements.insert(std::make_shared<GUI::SliderI>("texchoice", 1,0,0,15));
    gui->elements.insert(std::make_shared<GUI::SliderF>("blurv"));
    gui->elements.insert(std::make_shared<GUI::SliderF>("u_scale", 2,1));
    gui->elements.insert(std::make_shared<GUI::SliderF>("u_translate", 2,0,-1,1));

    for (auto e:gui->elements){ e.get()->links.insert(shader);}

    Atlas atlas("assets/media/");
    atlas.link(shader);

    Texture passBuf(FW,FH, GL_RGB8);

    Texture outBuf(FW,FH, GL_RGB8);
    FrameBuffer outFB(outBuf);

    FrameBuffer winFB(0);

    auto mat = matrice(10,1);
    UBO matriceUBO(&mat[0], mat.size()*sizeof(RectF), "MatriceUBO"); 
    matriceUBO.link(shader);
    matriceUBO.send();

    UBO fixtureUBO(&fixtures[0], 10*sizeof(FixtureUBO), "FixtureUBO"); 
    fixtureUBO.link(shader);
    fixtureUBO.send();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA); // OR glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    VBO quad;
    quad.addQuad(1); // UID #1 in shader (feedback)
    quad.addQuad(2); // UID #2 in shader (fixture)
    
    while(true) window->render([&]() {

        outFB.clear(); // thus bind

        fixtureUBO.send();

        passBuf.bind();
        shader->use();
        quad.draw(*quantity); // quantity is instances count in shader

        passBuf.copy(outBuf); 

        winFB.clear(); // thus bind
        Draw2D(outBuf);
    
        gui->draw();
 
    });

} 

