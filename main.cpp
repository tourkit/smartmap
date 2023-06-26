/*

    SMARTMAP (beta)

                        */


#include "smartmap.hpp"

#define DEBUG

// #include <windows.h>
// #include <ctime>
// #include <cstdint>

// int last_mil(const char* path, std::function<void()> cb = [](){}, int before = 0) {

//     WIN32_FILE_ATTRIBUTE_DATA fileInfo;
//     GetFileAttributesExA(path, GetFileExInfoStandard, &fileInfo);
//     SYSTEMTIME st;
//     FileTimeToSystemTime(&fileInfo.ftLastWriteTime, &st);

//     auto now = st.wMilliseconds;
//     if (before != now ) cb();
//     return now;

// }


int main() {  
    
    while(true) sm.window.render([&]() {

        glBindTexture(GL_TEXTURE_2D, sm.tex.id);
        sm.shader.use();
        sm.quad.draw();
        sm.gui.draw2();

    }); 

    // uint8_t min = 0, max = 255;

    // int fileCheck1 = 0;
    // int fileCheck2 = last_mil(quadfile);

    // int cells_count = 32;

    // while(true) sm.window->render([&]() {

        // fileCheck1 = last_mil(("C:/msys64/home/SysErr/old/smartmap/assets/shader/"+std::string(frag)).c_str(), [](){ ShaderProgram::pool[0]->reset(); }, fileCheck1);
        // fileCheck2 = last_mil(quadfile, [](){ VBO::pool[0]->reset(); }, fileCheck2);

        // Draw2D(tex);
        // for (auto q : VBO::pool) {

            // DrawCall(*q, tex);
            

        // }

        // sm.artnet.run();



        // CLUSTER RENDER LOOP


        // // END OF LOOP
    

}