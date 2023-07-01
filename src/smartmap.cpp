
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

void SmartMap::render() {
    while(true) sm.window->render([&]() {

        // debug.run();

        sm.artnet->run();

        sm.outFB->clear(); // thus bind

        sm.passBuf->bind();
        sm.shader->use();

        glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_SRC_COLOR);
        sm.quadA->draw(sm.MATS); // quantity is instances count in shader 
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        sm.quadB->draw(sm.MATS); // quantity is instances count in shader 

        // sm.passBuf->copy(*sm.outBuf);

        // glBindImageTexture(0, *sm.outBlur, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
        // glBindImageTexture(1, *sm.outBuf, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
        // sm.blur_x->use(sm.FW*.5/16,sm.FH*.5/16);
        // sm.blur_y->use(sm.FW*.5/16,sm.FH*.5/16);
        // glMemoryBarrier( GL_ALL_BARRIER_BITS ); 

        sm.winFB->clear(); 
        
        sm.outBuf->bind();
        sm.shader->use();
        sm.quadA->draw();

        sm.gui->draw2();  

    }); 
}

void SmartMap::createFixtures(int count) {

    winFB = new FrameBuffer(0); 

    // fixtures.resize(count);
    
    auto mat = matrice(MAT_X,MAT_Y);
    matriceUBO = new UBO("MatriceUBO", mat.size()*16, {shader->id}); 
    matriceUBO->update(&mat[0][0],mat.size()*16); 

    shader->sendUniform("MatriceUBOSize", MAT_X*MAT_Y);

    fixtureUBO = new UBO("FixtureUBO", 24*64, {shader->id}); 

    std::cout << "fin" << std::endl;
    
} 

SmartMap& SmartMap::getInstance() { static SmartMap instance;  return instance; }
SmartMap& sm = SmartMap::getInstance();