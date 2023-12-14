#include "output.hpp"  

#include "../stack.hpp"
#include "../smartmap.hpp"

namespace SmartMap {

VideoOutput::VideoOutput(std::string name, int width, int height) : Output(name, width,height) {
    
    cue = new Stack::Action([this](){

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0,0,this->fb.width,this->fb.height);

        this->fb.texture->bind();
        Engine::getInstance().basicshader->use();
        Engine::getInstance().quad->draw();

    }, name);
    
    Base::stack->childrens.back()->list.push_back(cue);

}

NDIOutput::NDIOutput(std::string name, int width, int height) : Output(name, width,height), ndisender(width,height) {

    data.resize(1920*1080*4);

    cue = new Stack::Action([this](){

        glBindFramebuffer(GL_FRAMEBUFFER, this->fb.id);

        // glPixelStorei(GL_PACK_ALIGNMENT, 1);

        glReadPixels(0,0, 1920, 1080, GL_BGRA, GL_UNSIGNED_BYTE, &this->data[0]);

        this->ndisender.send(&this->data[0],this->data.size());

    }, name);
    

    Base::stack->childrens.back()->list.push_back(cue);

}

};