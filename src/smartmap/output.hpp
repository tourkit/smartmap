#pragma once
 
#include "../framebuffer.hpp"
#include "../ndi.hpp"
#include "../engine.hpp"

namespace SmartMap {


struct Output {

    std::string name = "SmartMap Output";

    FrameBuffer *fb;

    enum TYPE { HDMI, NDI, DMX } type;

    void virtual draw() {}

};


struct VideoOutput : Output {

    VideoOutput(int width, int height, int offset_x = 0, int offset_y = 0) {

        name = "Video Output";

        fb = Engine::getInstance().fb;

    }

};


struct NDIOutput : Output {

    NDI::Sender ndisender;

    Stack::Cue * cue;

    NDIOutput(int width, int height);

};

};
