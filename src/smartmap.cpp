
#include "smartmap.hpp"

SmartMap::SmartMap() {

    std::cout << "deb" << std::endl;

    // VBO quadC("quad.obj");

    // order matters for some
    artnet = new Artnet("2.0.0.222");
    window = new GL::Window(false,400,300);
    window->setPos(2560,290);
    window->setSize(1920,1200);
    MAT_X = 1; 
    MAT_Y = 1;
    MATS = MAT_X*MAT_Y;
    FW = window->width*MAT_X;
    FH = window->height*MAT_Y;
    gui = new GUI(window->window);
    quadC = new VBO();
    quadA = new VBO("quad.obj",2);
    quadB = new VBO("quad.obj",1);
    passBuf = new Texture(nullptr,FW,FH);
    passBuf->format = GL_RGBA8;
    
    outBuf = new Texture(nullptr, FW,FH); 
    outBuf->format = GL_RGBA8;
    
    outFB = new FrameBuffer(*outBuf); 

    outBlur = new Texture(nullptr, FW*.5,FH*.5); 
    outBlur->format = GL_RGBA8;

    shader = new ShaderProgram({"smartmap.frag", "smartmap.vert"});
    blur_x = new ShaderProgram({"blur_x.comp"});
    blur_y = new ShaderProgram({"blur_y.comp"});
    basic = new ShaderProgram({"test.frag", "basic.vert"});

    
    shader->use();
    atlas = new Atlas("assets/media/");
    atlas->link(shader);

}

void SmartMap::createFixtures(int count) {

    winFB = new FrameBuffer(0); 

    fixtures.resize(count);
    
    auto mat = matrice(MAT_X,MAT_Y);
    matriceUBO = new UBO("MatriceUBO", mat.size()*16, {shader->id}); 
    matriceUBO->update(&mat[0][0],mat.size()*16); 

    shader->sendUniform("MatriceUBOSize", MAT_X*MAT_Y);

    fixtureUBO = new UBO("FixtureUBO", 24*64, {shader->id}); 

    std::cout << "fin" << std::endl;
    
} 

SmartMap& SmartMap::getInstance() { static SmartMap instance;  return instance; }
SmartMap& sm = SmartMap::getInstance();