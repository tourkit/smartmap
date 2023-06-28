/*

    SMARTMAP (beta)

                        */


#include "smartmap.hpp"

#define DEBUG

#include <windows.h>
#include <ctime>
#include <cstdint>

int last_mil(const char* path, std::function<void()> cb = [](){}, int before = 0) {

    WIN32_FILE_ATTRIBUTE_DATA fileInfo; GetFileAttributesExA(path, GetFileExInfoStandard, &fileInfo);
    SYSTEMTIME st; FileTimeToSystemTime(&fileInfo.ftLastWriteTime, &st);

    auto now = st.wMilliseconds;
    if (before != now ) cb();
    return now;

}

int fileCheck1 = 0;
int fileCheck2 = 0;
int fileCheck3 = 0;

int main() {  

    sm.createFixtures(1);

    while(true) sm.window->render([&]() {

        fileCheck1 = last_mil(("C:/msys64/home/SysErr/old/smartmap/assets/shader/"+std::string(sm.shader->paths[0])).c_str(), [](){ sm.shader->reset(); }, fileCheck1);
        fileCheck2 = last_mil(("C:/msys64/home/SysErr/old/smartmap/assets/shader/"+std::string(sm.shader->paths[1])).c_str(), [&](){ sm.shader->reset(); }, fileCheck2);
        // fileCheck3 = last_mil(("C:/msys64/home/SysErr/old/smartmap/assets/model/"+std::string(sm.quadA->path)).c_str(), [](){ VBO::pool[0]->reset(); }, fileCheck3);

        sm.outFB->clear(); // thus bind

        sm.fixtureUBO->send();

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
        
        glBindTexture(GL_TEXTURE_2D, sm.outBuf->id);
        sm.basic->use();
        sm.quadA->draw();

        sm.gui->draw2();

        sm.artnet->run();

    }); 

 
}