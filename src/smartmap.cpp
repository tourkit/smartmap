
#include "smartmap.hpp"
#include "stack.hpp"

#include "imgui/imgui.h"
#include "smartmap/layer.hpp"

#include "include/vendor/rapidjson/document.h"
#include "include/vendor/rapidjson/stringbuffer.h"

namespace SmartMap {

Base::Base() {

    artnet = new Artnet{"2.0.0.222"};
    shader = new ShaderProgram({"smartmap.frag", "smartmap.vert"});
    
    layershader = new ShaderProgram({"layer.frag", "basic.vert"});

    shader->sendUniform("freetype", 2);
            
    auto &window = Engine::getInstance().window;
    window.setPos(2560,0);
    window.setSize(1920,1080);
              
    shader->use();
    atlas = new Atlas("assets/media/",4096,2048);
    atlas->link(shader);
    
    smartlayersUBO = engine.static_ubo.buffer.add("Layer", {"int", "ID", "Offset", "ID"}, 10 );

    fix1UBO = engine.dynamic_ubo.buffer.add("Fixture", {

        "Opacity",
        "RGB",
        "Position",
        "Size",
        "Gobo",
        "Orientation",
        "Feedback",
        "Strobe",
        "float", // for alignmentr
        
    }, 50 );

    fix2UBO = engine.dynamic_ubo.buffer.add("Fixture2", {

        "Opacity",
        "RGB",
        "Position",
        "Size",
        "Gobo",
        "Orientation",
        "Feedback",
        "Strobe",
        "float", // for alignmentr
        
    }, 50 );
    
    // blur_x = new ShaderProgram({"blur_x.comp"});
    // blur_y = new ShaderProgram({"blur_y.comp"});
    // basic = new ShaderProgram({"test.frag", "basic.vert"});
    // outBlur = new Texture(nullptr, FW*.5,FH*.5); 
    // outBlur->format = GL_RGBA8;

     
    engine.stack.list.push_back(new Stack::Action{[this](){

        memcpy(fix2UBO->data(),fix1UBO->data(),fix1UBO->byte_size*fix1UBO->quantity);
        artnet->run();

    }, "Artnet"});
            
    engine.stack.list.push_back(new Stack::Action{[this](){
  
        for (auto layer:SmartMap::Layer::pool) { 
            
            shader->use();
            
            layer->fb->clear(); // thus bind
            
            glBlendFunc(GL_BLEND_MODES[GL_BLEND_MODE_IN2], GL_BLEND_MODES[GL_BLEND_MODE_OUT2]);
            layer->pass->bind();
            atlas->texture->bind();
            layer->FTbuffer->bind();
            
            layer->quad->draw(layer->quantity); 
            
            if (shader->loaded) layer->pass->read(layer->fb->texture);
            
        }

    }, "SM A"});

    engine.stack.list.push_back(new Stack::Action{[this](){
        
        Engine::getInstance().fb->bind();
            
        layershader->use();

        for (auto layer:SmartMap::Layer::pool) { 

            layer->fb->texture->bind();
            
            layer->quad->draw();
            
        }

        //  fix2UBO->update();

    }, "SM B"});

            
} 

}; 