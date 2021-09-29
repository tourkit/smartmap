/*

    SMARTMAP v1 beta

                        */

#include "smartmap.hpp"

int WIDTH = 600, HEIGHT = 300;

int MAT_X = 1; 
int MAT_Y = 1;

auto* window = new GL::Window(false,WIDTH,HEIGHT,1920-WIDTH);

auto*  gui = new GUI{window->window}; 

auto* shader = new ShaderProgram({"assets/shader/smartmap.vert", "assets/shader/smartmap.frag"});
auto* blur_x = new ShaderProgram({"assets/shader/blur_x.comp"});
auto* blur_y = new ShaderProgram({"assets/shader/blur_x.comp"});
 
Quad draw2D;
void Draw2D(const Texture& tex) {

    glBindTexture(GL_TEXTURE_2D, tex.id);
    shader->use();
    draw2D.draw();

}

int main() {

    Atlas atlas("assets/media/");
    atlas.link(shader);

    
    int FW = 1920*MAT_X, FH = 1200*MAT_Y;

    Texture passBuf(FW,FH, GL_RGBA8);

    Texture outBuf(FW,FH, GL_RGBA8); 
    FrameBuffer outFB(outBuf);
    Texture outBlur(FW*.5,FH*.5, GL_RGBA8); 

    FrameBuffer winFB(0);

    VBO quad;
    quad.addQuad(1); // UID #1 in shader (feedback)
    quad.addQuad(2); // UID #2 in shader (fixture)

    auto mat = matrice(MAT_X,MAT_Y);
    UBO matriceUBO(&mat[0], mat.size()*sizeof(RectF), "MatriceUBO"); 
    matriceUBO.link(shader);
    matriceUBO.send();

    struct FixtureUBO { vec2 focus{.1,.9}, pos{0,0}; vec4 rgba = {1,1,1,1}; vec4 gobo; float orientation = .0; float feedback =0; float strobe;  float ratio = 600./300.; } fixtures[24];
    UBO fixtureUBO(&fixtures[0], 24*sizeof(FixtureUBO), "FixtureUBO"); 
    fixtureUBO.link(shader);
    fixtureUBO.send();

    float selectedf = 0;

    auto count = gui->add(new GUI::Counter("count"), shader);
    auto blur = gui->add(new GUI::SliderF("blurv",   1, .0,  0,  1));
    blur->links.insert(blur_x);
    blur->links.insert(blur_y);
    

    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_SRC_COLOR);

    glEnable(GL_CLIP_DISTANCE0);
    glEnable(GL_CLIP_DISTANCE1);
    glEnable(GL_CLIP_DISTANCE2);
    glEnable(GL_CLIP_DISTANCE3);

    shader->sendUniform("fixcount", MAT_X*MAT_Y);

    while(true) window->render([&]() {

        gui->elements.resize(2);
        gui->add(new GUI::SliderI("selected", 1,  0,  0,  23, &selectedf)); int selected = selectedf;
        gui->add(new GUI::SliderF("size",     2, .1,  0,  1, &fixtures[selected].focus.x));
        gui->add(new GUI::SliderF("position", 2,  0, -1,  1, &fixtures[selected].pos.x));
        gui->add(new GUI::SliderF("Angle",    1, .0,  0,  1, &fixtures[selected].orientation));
        gui->add(new GUI::SliderF("rgba",     3,  1,  0,  1, &fixtures[selected].rgba.x));
        gui->add(new GUI::SliderF("feedback", 1, .9,  0,  1, &fixtures[selected].feedback));
        gui->add(new GUI::SliderF("gobo",     1,  0,  0,  1, &fixtures[selected].gobo.x));
        gui->add(new GUI::SliderF("gobo_fx",  3,  0,  0,  1, &fixtures[selected].gobo.y));
        gui->add(new GUI::SliderF("strobe",   1, .9,  0,  1, &fixtures[selected].strobe));

        // CLUSTER RENDER LOOP

        outFB.clear(); // thus bind

        fixtureUBO.send();

        passBuf.bind();
        shader->use();
        quad.draw(MAT_X*MAT_Y); // quantity is instances count in shader

        passBuf.copy(outBuf);

        glBindImageTexture(0, outBlur, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
        glBindImageTexture(1, outBuf, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
        blur_x->use(FW*.5/16,FH*.5/16);
        blur_y->use(FW*.5/16,FH*.5/16);
        glMemoryBarrier( GL_ALL_BARRIER_BITS ); 

        winFB.clear(); // thus bind
        Draw2D(outBlur);

        // // END OF LOOP
    
        gui->draw();
 
    });

} 

