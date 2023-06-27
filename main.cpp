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

    int MAT_X = 5,MAT_Y = 2;
    auto mat = matrice(MAT_X,MAT_Y);
    UBO matriceUBO(&mat[0], mat.size()*sizeof(RectF), "MatriceUBO"); 
    matriceUBO.link(&sm.shader);
    matriceUBO.send();

    // atlas.link(&sm.shader);

    while(true) sm.window.render([&]() {

        fileCheck1 = last_mil(("C:/msys64/home/SysErr/old/smartmap/assets/shader/"+std::string(sm.basic.paths[0])).c_str(), [](){ sm.basic.reset(); }, fileCheck1);
        fileCheck2 = last_mil(("C:/msys64/home/SysErr/old/smartmap/assets/shader/"+std::string(sm.basic.paths[1])).c_str(), [&](){ sm.basic.reset(); }, fileCheck2);
        fileCheck3 = last_mil(("C:/msys64/home/SysErr/old/smartmap/assets/model/"+std::string(sm.quadA.path)).c_str(), [](){ VBO::pool[0]->reset(); }, fileCheck3);

        glBindTexture(GL_TEXTURE_2D, sm.tex.id);
        sm.basic.use();
        sm.quadA.draw();
        sm.gui.draw2();

        sm.artnet.run();

    }); 

 
}