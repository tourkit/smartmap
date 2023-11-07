#include "output.hpp"  

#include "../stack.hpp"

namespace SmartMap {

VideoOutput::VideoOutput(int width, int height, int offset_x, int offset_y) : Output("Video Output", width,height,offset_x,offset_y) {
    
    cue = new Stack::Action([this](){

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(this->offset_x,this->offset_y,fb.width,fb.height);

        this->fb.texture->bind();
        Engine::getInstance().basicshader->use();
        Engine::getInstance().quad->draw();

    }, name);
    
    engine.stack.list.push_back(cue);

}

NDIOutput::NDIOutput(int width, int height, int offset_x, int offset_y) : Output("NDI Output", width,height,offset_x,offset_y), ndisender(width,height) {

    

    cue = new Stack::Action([this](){

        std::vector<unsigned char> data;
        data.resize(1920*1080*4);

        glBindFramebuffer(GL_FRAMEBUFFER, this->fb.id);

        glReadPixels(0,0, 1920, 1080, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);

        this->ndisender.send(&data[0],data.size());

    }, name);
    
    engine.stack.list.push_back(cue);

}

};