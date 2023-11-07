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

    Output(std::string name, int width, int height) 
        
        : name(name),fb(width,height) { 
    
        pool.push_back(this) ; 
    
    }

    enum TYPE { HDMI, NDI, DMX } type;

    void virtual draw() {}

};


struct VideoOutput : Output {

    VideoOutput(std::string name, int width, int height);

};

struct NDIOutput : Output {

    NDI::Sender ndisender;

    NDIOutput(std::string name, int width, int height);

};

};
