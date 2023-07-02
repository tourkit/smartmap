
#include "smartmap.hpp"

SmartMap::SmartMap() {

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
    quadC = new VBO("quad.obj",0);
    quadA = new VBO("quad.obj",2);
    quadB = new VBO("quad.obj",1);
    passBuf = new Texture(nullptr,FW,FH);
    passBuf->format = GL_RGBA8;
    
    outBuf = new Texture(nullptr, FW,FH); 
    outBuf->format = GL_RGBA8;
    
    outFB = new FrameBuffer(outBuf); 

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

void SmartMap::createFixtures(int count, GLuint chan, GLuint uni, Fixture *fixture) {

    winFB = new FrameBuffer(80085); 
    
    auto mat = matrice(MAT_X,MAT_Y);
    matriceUBO = new UBO("MatriceUBO", mat.size()*16, {shader->id}); 
    matriceUBO->update(&mat[0][0],mat.size()*16); 

    shader->sendUniform("MatriceUBOSize", MAT_X*MAT_Y);

    fixtureUBO = new UBO("FixtureUBO", 24*64, {shader->id}); 

    artnet->universes[0].callback = [&](Artnet::Universe* _this) {  artnet->universes[0].remap(&fixtureUBO->data[0], *fixture); fixtureUBO->update(); };
    
} 

#ifdef SM_DEBUG
#include <windows.h>
#include <ctime>
#include <cstdint>

static inline std::map<int,int> filechecks;

static int last_mil(const char* path, std::function<void()> cb = [](){}, int before = 0) {

    WIN32_FILE_ATTRIBUTE_DATA fileInfo; GetFileAttributesExA(path, GetFileExInfoStandard, &fileInfo);
    SYSTEMTIME st; FileTimeToSystemTime(&fileInfo.ftLastWriteTime, &st);

    auto now = st.wMilliseconds;
    if (before != now ) cb();
    return now;

}

static inline int survey_count = 0;

static inline void survey(const char* path, std::function<void()> cb = [](){}) {

    WIN32_FILE_ATTRIBUTE_DATA fileInfo; GetFileAttributesExA(path, GetFileExInfoStandard, &fileInfo);
    SYSTEMTIME st; FileTimeToSystemTime(&fileInfo.ftLastWriteTime, &st);

    auto now = st.wMilliseconds;

    if (filechecks[survey_count] != now ) cb();

    filechecks[survey_count] = now;

}

#endif

void SmartMap::render() {

    while(true) sm.window->render([&]() {

#ifdef SM_DEBUG
        survey_count = 0;
        survey(("C:/msys64/home/SysErr/old/smartmap/assets/shader/"+std::string(sm.shader->paths[0])).c_str(), [&](){ shader->reset(); atlas->link(shader); }); 
        survey(("C:/msys64/home/SysErr/old/smartmap/assets/shader/"+std::string(sm.shader->paths[1])).c_str(), [&](){ shader->reset(); atlas->link(shader); });
#endif

        sm.artnet->run(); 
        sm.artnet->universes[0].update(); 

        sm.outFB->clear(); // thus bind

        sm.passBuf->bind();

        // feedback
        glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_SRC_COLOR);
        sm.quadA->draw(); // quantity is instances count in shader 
        glBlendFunc(sm.GL_BLEND_MODES[sm.GL_BLEND_MODE_IN], sm.GL_BLEND_MODES[sm.GL_BLEND_MODE_OUT]);
        
        // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        sm.quadB->draw(); // quantity is instances count in shader 

        sm.passBuf->copy(sm.outBuf);

        // glBindImageTexture(0, *sm.outBlur, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
        // glBindImageTexture(1, *sm.outBuf, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
        // sm.blur_x->use(sm.FW*.5/16,sm.FH*.5/16);
        // sm.blur_y->use(sm.FW*.5/16,sm.FH*.5/16);
        // glMemoryBarrier( GL_ALL_BARRIER_BITS ); 

        sm.winFB->clear(); 
        
        sm.outBuf->bind();
        sm.shader->use();
        sm.quadC->draw();

        sm.gui->draw2();  

    }); 

}

SmartMap& SmartMap::getInstance() { static SmartMap instance;  return instance; }
SmartMap& sm = SmartMap::getInstance();