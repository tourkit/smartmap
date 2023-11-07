#pragma once
 
#include "../framebuffer.hpp"
#include "../ndi.hpp"
#include "../stack.hpp"
#include "../engine.hpp"

namespace SmartMap {


struct Output {

    std::string name = "SmartMap Output";

    FrameBuffer *fb;

    enum TYPE { HDMI, NDI, DMX } type;

    void virtual draw() {}

};


struct VideoOutput : Output {

    NDIOutput(int width, int height, int offset_x = 0, int offset_y = 0) {

        name = "Video Output"

        fb = Engine::getInstance().fb;

    }

};


struct NDIOutput : Output {

    NDI::Sender ndisender;

    Stack::Cue * cue;

    NDIOutput(int width, int height) : ndisender(width,height) {

        name = "NDI Output";

        fb = new FrameBuffer(width,height);
    
        cue = new Stack::Action([this](){

            std::vector<unsigned char> data;
            data.resize(1920*1080*4);

            glBindFramebuffer(GL_FRAMEBUFFER, this->id);

            glReadPixels(0,0, 1920, 1080, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);

            this->ndisender->send(&data[0],data.size());

        }, name);
        
        engine.stack.list.push_back(cue);

    }

};

};


#endif