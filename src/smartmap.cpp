
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
    // engine.stack.list.back()->active=false;

            
} 




 
// void SmartMap::render() {

    // while(true) window->render([&]() {

    //     for (int i = 0; i < 10; i++) shader->sendUniform("debug"+std::to_string(i), debuguniforms[i]);

    //     artnet->run(); 

    //     memcpy(&fixtureUBO2->data[0],&fixtureUBO->data[0],fixtureUBO->data.size()*4);

    //     winFB->clear(); 

    //     shader->sendUniform("strobe", frame++%256);

    //     int offset = 0;

    //     for (auto layer:SmartMap::Layer::pool) { 

    //         layer->fb->clear(); // thus bind

    //         shader->sendUniform("offset", offset);
    //         offset+=layer->quantity;
    //         shader->sendUniform("mode", ((layer->mode==Layer::Mode::Grid)?1.0f:0.0f));
    //         shader->sendUniform("MatriceUBOSize", layer->quantity);
            
    //         layer->FTbuffer->bind();
    //         glBlendFunc(GL_BLEND_MODES[GL_BLEND_MODE_IN2], GL_BLEND_MODES[GL_BLEND_MODE_OUT2]);
    //         layer->quadA->draw(layer->quantity); 

    //         layer->pass->bind();
    //         glBlendFunc(GL_BLEND_MODES[GL_BLEND_MODE_IN], GL_BLEND_MODES[GL_BLEND_MODE_OUT]);
    //         layer->quadB->draw(layer->quantity);

    //         layer->pass->read(layer->buffer);

    //         // glBindImageTexture(0, *outBlur, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
    //         // glBindImageTexture(1, *outBuf, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
    //         // blur_x->use(FW*.5/16,FH*.5/16);
    //         // blur_y->use(FW*.5/16,FH*.5/16);
    //         // glMemoryBarrier( GL_ALL_BARRIER_BITS ); 
            
    //         winFB->bind(); 
    //         layer->buffer->bind();
    //         shader->use();
    //         quad->draw();

    //     }

    //     fixtureUBO2->upload();





        // --------------------------



        //////////////////////////////////////////////
        //////////////////////////////////////////////
        //////////////////////////////////////////////
        // GUI STUFF
        //////////////////////////////////////////////
        //////////////////////////////////////////////

        // ///// BUFFERS

        // ImGui::Begin("KTRL");
        
        //     // ImGui::SetWindowFontScale(1.5);

        //     ImGui::Checkbox("Debug mode", &debug);
            
        //     uint16_t min = 1, max = 2560;

        //     if (ImGui::DragScalarN("winsize", ImGuiDataType_U16,  &window->width,  2, 1, &min,&max)) window->updateSize();
        //     if (ImGui::DragScalarN("winpos", ImGuiDataType_U16,  &window->offset_x,  2, 1, &min,&max)) window->updatePos();

        //     ImGui::Separator();
            
        //     for (auto fps:FPS::pool) if (fps->fps > 1) ImGui::Text((fps->name+": "+std::to_string((GLuint)(fps->fps))+" FPS").c_str());
            
        //     ImGui::Separator();
            
        //     for (int i = 0; i < 5; i++) ImGui::SliderFloat(("debug "+std::to_string(i)).c_str(), &debuguniforms[i], 0, 1);

        //     ImGui::Separator();

        //     ImGui::SliderInt("GL_BLEND_MODE_IN2",&GL_BLEND_MODE_IN2,0,GL_BLEND_MODES.size());
        //     ImGui::SliderInt("GL_B2LEND_MODE_OUT2",&GL_BLEND_MODE_OUT2,0,GL_BLEND_MODES.size()); 
        //     ImGui::SliderInt("GL_BLEND_MODE_IN",&GL_BLEND_MODE_IN,0,GL_BLEND_MODES.size());
        //     ImGui::SliderInt("GL_B2LEND_MODE_OUT",&GL_BLEND_MODE_OUT,0,GL_BLEND_MODES.size()); 
        //     ImGui::SliderInt("GL_BLEND_MODE_IN3",&GL_BLEND_MODE_IN3,0,GL_BLEND_MODES.size());
        //     ImGui::SliderInt("GL_B2LEND_MODE_OUT3",&GL_BLEND_MODE_OUT3,0,GL_BLEND_MODES.size()); 
            
        // ImGui::End();



//     }); 

// }

// SmartMap& SmartMap::getInstance() { static SmartMap instance;  return instance; }

    }; 