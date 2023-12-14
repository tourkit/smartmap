#include "output.hpp"  

#include "../stack.hpp"
#include "../smartmap.hpp"

namespace SmartMap {

Output::Output(std::string name, int width, int height) 
        
        : name(name),fb(width,height) { 
    
        pool.push_back(this) ; 

        PLOGD << name;
    
    }

Output::~Output() { 
        
        delete cue; 

        auto it = std::find(pool.begin(), pool.end(), this);
        if (it != pool.end()) pool.erase(it);

        PLOGD << name;
        
    }

VideoOutput::~VideoOutput() {

        auto it = std::find( Base::stack->childrens.back()->list.begin(),  Base::stack->childrens.back()->list.end(), cue);
        if (it !=  Base::stack->childrens.back()->list.end())  Base::stack->childrens.back()->list.erase(it);
        else PLOGW << "CROUTE";

}

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

    data.resize(width*height*4);

    cue = new Stack::Action([this](){

        glBindFramebuffer(GL_FRAMEBUFFER, this->fb.id);

        // glPixelStorei(GL_PACK_ALIGNMENT, 1);

        glReadPixels(0,0, fb.width, fb.height, GL_BGRA, GL_UNSIGNED_BYTE, &this->data[0]);

        this->ndisender.send(&this->data[0], this->data.size());

    }, name);
    

    Base::stack->childrens.back()->list.push_back(cue);

}

};
