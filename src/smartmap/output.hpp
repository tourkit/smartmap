#pragma once
 
#include "../framebuffer.hpp"
#include "../ndi.hpp"
#include "../engine.hpp"

namespace SmartMap {


struct Output {

    static inline std::vector<Output*> pool;

    std::string name = "SmartMap Output";

    FrameBuffer fb;

    Stack::Cue * cue;

    int offset_x, offset_y;

    Output(std::string name, int width, int height, int offset_x = 0, int offset_y = 0) 
        
        : name(name),fb(width,height), offset_x(offset_x), offset_y(offset_y) { 
    
        pool.push_back(this) ; 
    
    }

    enum TYPE { HDMI, NDI, DMX } type;

    void virtual draw() {}

};


struct VideoOutput : Output {

    VideoOutput(int width, int height, int offset_x = 0, int offset_y = 0);

};

struct NDIOutput : Output {

    NDI::Sender ndisender;

    NDIOutput(int width, int height, int offset_x = 0, int offset_y = 0);

};

};
