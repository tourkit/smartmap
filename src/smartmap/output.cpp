#include "output.hpp"  

#include "../stack.hpp"

namespace SmartMap {

NDIOutput::NDIOutput(int width, int height) : ndisender(width,height) {

    name = "NDI Output";

    fb = new FrameBuffer(width,height);

    cue = new Stack::Action([this](){

        std::vector<unsigned char> data;
        data.resize(1920*1080*4);

        glBindFramebuffer(GL_FRAMEBUFFER, this->fb->id);

        glReadPixels(0,0, 1920, 1080, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);

        this->ndisender.send(&data[0],data.size());

    }, name);
    
    engine.stack.list.push_back(cue);

}

};