/*

    SMARTMAP v1 beta

                        */

#include "smartmap.hpp"

int WIDTH = 600, HEIGHT = 300;

int FW = 6000, FH = 300;

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

    Atlas atlas("assets/media/");
    atlas.link(shader);

    Texture passBuf(FW,FH, GL_RGB8);

    Texture outBuf(FW,FH, GL_RGB8); 
    FrameBuffer outFB(outBuf);

    FrameBuffer winFB(0);

    VBO quad;
    quad.addQuad(1); // UID #1 in shader (feedback)
    quad.addQuad(2); // UID #2 in shader (fixture)

    auto mat = matrice(10,1);
    UBO matriceUBO(&mat[0], mat.size()*sizeof(RectF), "MatriceUBO"); 
    matriceUBO.link(shader);
    matriceUBO.send();

    struct FixtureUBO { vec2 focus{.1,1}, pos{0,0}; vec4 rgba = {1,1,1,1}; vec4 gobo; float feedback; float orientation; float strobe;  float useless; } fixtures[10];
    UBO fixtureUBO(&fixtures[0], 10*sizeof(FixtureUBO), "FixtureUBO"); 
    fixtureUBO.link(shader);
    fixtureUBO.send();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA); // OR glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float selectedf = 0;

    gui->add(new GUI::Counter("count"), shader);

    while(true) window->render([&]() {

        gui->elements.resize(0);
        gui->add(new GUI::SliderI("selected", 1,  0,  0,  9, &selectedf)); int selected = selectedf;
        gui->add(new GUI::SliderF("size",      2,  1,  0,  1, &fixtures[selected].focus.x));
        gui->add(new GUI::SliderF("position",  2,  0, -1,  1, &fixtures[selected].pos.x));
        gui->add(new GUI::SliderF("Orientation",1,.5,  0,  1, &fixtures[selected].orientation));
        gui->add(new GUI::SliderF("rgba",      3,  1,  0,  1, &fixtures[selected].rgba.x));
        gui->add(new GUI::SliderF("feedback",  1, .9,  0,  1, &fixtures[selected].feedback));
        gui->add(new GUI::SliderF("gobo",      1,  0,  0,  1, &fixtures[selected].gobo.x));
        gui->add(new GUI::SliderF("gobo_fx",   3,  0,  0,  1, &fixtures[selected].gobo.y));
        gui->add(new GUI::SliderF("strobe",    1, .9,  0,  1, &fixtures[selected].strobe));

        outFB.clear(); // thus bind

        fixtureUBO.send();

        passBuf.bind();
        shader->use();
        quad.draw(10); // quantity is instances count in shader

        passBuf.copy(outBuf); 

        winFB.clear(); // thus bind
        Draw2D(outBuf);
    
        gui->draw();
 
    });

} 

