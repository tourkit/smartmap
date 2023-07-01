#pragma once

#include "smartmap.hpp"


#include <windows.h>
#include <ctime>
#include <cstdint>

struct Debug {

int fileCheck3 = 0;
int fileCheck4 = 0;

int last_mil(const char* path, std::function<void()> cb = [](){}, int before = 0) {

    WIN32_FILE_ATTRIBUTE_DATA fileInfo; GetFileAttributesExA(path, GetFileExInfoStandard, &fileInfo);
    SYSTEMTIME st; FileTimeToSystemTime(&fileInfo.ftLastWriteTime, &st);

    auto now = st.wMilliseconds;
    if (before != now ) cb();
    return now;

}

void reset() {

    sm.shader->reset();     

    GLuint textureUniformLoc = glGetUniformLocation(sm.shader->id, "mediasAtlas");
    glUniform1i(textureUniformLoc, 1);

}

void run() {

    // fileCheck3 = last_mil(("C:/msys64/home/SysErr/old/smartmap/assets/shader/"+std::string(sm.shader->paths[0])).c_str(), [&](){ reset(); }, fileCheck3);
    // fileCheck4 = last_mil(("C:/msys64/home/SysErr/old/smartmap/assets/shader/"+std::string(sm.shader->paths[1])).c_str(), [&](){ reset(); }, fileCheck4);

}

};
