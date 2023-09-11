
#include "smartmap.hpp"

#include "imgui/imgui.h"

#include <filesystem>

namespace fs = std::filesystem;

#ifndef SM_DEBUG
#define SM_DEBUG
#endif

#ifdef SM_DEBUG

#include <ctime>
#include <cstdint>

static inline std::map<int,int> filechecks;
static inline int survey_count = 0;
static inline void survey(const char* path, std::function<void()> cb = [](){}) {

    static auto startTime = fs::file_time_type::clock::now();

    auto lastWriteTime = fs::last_write_time(path);
    auto mSecs = std::chrono::duration_cast<std::chrono::milliseconds>(lastWriteTime - startTime).count();

    if (filechecks[survey_count] != mSecs) {
        filechecks[survey_count] = mSecs;
        cb();
    }
    survey_count++;

}

#endif 

SmartMap::SmartMap() {

    artnet = new Artnet{"2.0.0.222"};
    shader = new ShaderProgram({"smartmap.frag", "smartmap.vert"});

    auto &window = Engine::getInstance().window;
    window.setPos(2560-400,0);
    window.setSize(400,300);
    
    shader->use();
    atlas = new Atlas("assets/media/",4096,2048);
    atlas->link(shader);

    engine.dynamic_ubo.subscribers.push_back(shader); 
    engine.static_ubo.subscribers.push_back(shader); 

    mat1UBO = engine.static_ubo.buffer.add("matriceUBO", {"Size", "Position"}, 24 );

    mat2UBO = engine.static_ubo.buffer.add("matriceUBO2", {"Size", "Position"}, 24 );

    fix1UBO = engine.dynamic_ubo.buffer.add("FixtureUBO", {

        "Opacity",
        "RGB",
        "Position",
        "Size",
        "Gobo",
        "Orientation",
        "Feedback",
        "Strobe",
        "float", // for alignmentr
        
    }, 24 );

    fix2UBO = engine.dynamic_ubo.buffer.add("FixtureUBO2", {

        "Opacity",
        "RGB",
        "Position",
        "Size",
        "Gobo",
        "Orientation",
        "Feedback",
        "Strobe",
        "float", // for alignmentr
        
    }, 24 );

    
    // blur_x = new ShaderProgram({"blur_x.comp"});
    // blur_y = new ShaderProgram({"blur_y.comp"});
    // basic = new ShaderProgram({"test.frag", "basic.vert"});
    // outBlur = new Texture(nullptr, FW*.5,FH*.5); 
    // outBlur->format = GL_RGBA8;

        basic_fixture.presets = { 

        {"DIMMER", {
            {"DIMMER", {
                {"Dim",         1,  &Component::id("Opacity")->members[0]},
        }}}},

        {"COLOR", {
            {"RGB", {
                {"Red",         1,  &Component::id("RGB")->members[0]}, 
                {"Green",       1,  &Component::id("RGB")->members[1]}, 
                {"Blue",        1,  &Component::id("RGB")->members[2]},                            
        }}}},

        {"Position", {
            {"Position", { 
                {"Pos_X",       2,  &Component::id("Position")->members[0]}, 
                {"Pos_Y",       2,  &Component::id("Position")->members[1]},                                      
        }}}},

        {"Focus", {
            {"Focus", {
                {"Focus_X",     2,  &Component::id("Size")->members[0]}, 
                {"Focus_Y",     2,  &Component::id("Size")->members[1]},                                  
        }}}},

        {"Gobo", {
            {"Gobo", {
                {"Gobo_ID",     1,  &Component::id("Gobo")->members[0]}, 
                {"Gobo_FX1",    1,  &Component::id("Gobo")->members[1]}, 
                {"Gobo_FX2",    1,  &Component::id("Gobo")->members[2]}, 
                {"Gobo_FX3",    1,  &Component::id("Gobo")->members[3]},
        }}}},

        {"Beam", {
            {"Beam", {
                {"Orientation", 2,  &Component::id("Orientation")->members[0]}, 
                {"Feedback",    1,  &Component::id("Feedback")->members[0]}, 
                {"Strobe",      1,  &Component::id("Strobe")->members[0]},               
        }}}},

    };

    engine.stack.list.push_back(new Engine::Stack::DrawCall{&engine.quad, shader});

    engine.stack.list.push_back(new Engine::Stack::Action{[this](){

        artnet->run();

    }});
#ifdef SM_DEBUG
    engine.stack.list.push_back(new Engine::Stack::Action{[this](){

            auto cb = [&](){ 

                shader->reset(); 

                atlas->link(shader); 

                Engine::getInstance().dynamic_ubo.upload();
                Engine::getInstance().static_ubo.upload();  

            };

            survey_count = 0;
            survey((fs::path(REPO_DIR) / "assets/shader" / shader->paths[0]).c_str(), cb);
            survey((fs::path(REPO_DIR) / "assets/shader" / shader->paths[1]).c_str(), cb);



    }});
#endif

    engine.stack.list.push_back(new Engine::Stack::Action{[this](){

        int offset = 0;

        for (auto layer:SmartMap::Layer::pool) { 

            layer->fb->clear(); // thus bind

            shader->sendUniform("offset", offset);
            offset+=layer->quantity;
            shader->sendUniform("mode", ((layer->mode==Layer::Mode::Grid)?1.0f:0.0f));
            shader->sendUniform("MatriceUBOSize", layer->quantity);
            
            layer->FTbuffer->bind();
            glBlendFunc(GL_BLEND_MODES[GL_BLEND_MODE_IN2], GL_BLEND_MODES[GL_BLEND_MODE_OUT2]);
            layer->quadA->draw(layer->quantity); 

            layer->pass->bind();
            glBlendFunc(GL_BLEND_MODES[GL_BLEND_MODE_IN], GL_BLEND_MODES[GL_BLEND_MODE_OUT]);
            layer->quadB->draw(layer->quantity);

            layer->pass->read(layer->buffer);

            // glBindImageTexture(0, *outBlur, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
            // glBindImageTexture(1, *outBuf, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
            // blur_x->use(FW*.5/16,FH*.5/16);
            // blur_y->use(FW*.5/16,FH*.5/16);
            // glMemoryBarrier( GL_ALL_BARRIER_BITS ); 
            
            Engine::getInstance().fb.bind(); 
            layer->buffer->bind();
            shader->use();
            Engine::getInstance().quad.draw();

        }

    }});

}

SmartMap::Layer::Layer(uint16_t chan, uint16_t uni, DMX::Fixture &fixture, uint16_t width, uint16_t height, Layer::Mode mode, uint16_t quantity_x, uint16_t quantity_y, float scale) 

    : chan(chan), uni(uni), width(width), height(height), mode(mode), quantity_x(quantity_x), quantity_y(quantity_y), quantity(quantity_x*quantity_y) {

    for (auto &layer:pool) { 

        for (auto c:fix1UBO->components) { attroffset += layer->quantity*c->members.size(); }

    }
    
    for (auto &layer:pool) { matoffset+=layer->quantity*4; }

    pool.push_back(this);
    
    GLuint FW = width*scale, FH = height*scale;
    if (mode == Layer::Mode::Free) { FW *= quantity_x; FH *= quantity_y; }

    std::vector<std::array<float, 4>> mat;
    
    mat = matrice(quantity_x,quantity_y);   
    memcpy(&engine.static_ubo.buffer.data[mat2UBO->offset],&mat[0][0],quantity*16);

    mat = matrice2(quantity_x,quantity_y);    
    memcpy(&engine.static_ubo.buffer.data[mat1UBO->offset],&mat[0][0],quantity*16);
    engine.static_ubo.upload(); 

    shader->sendUniform("MatriceUBOSize", quantity_x*quantity_y);

    quadB = new VBO("quad.obj", FW, FH);
    quadA = new VBO("quad.obj", FW, FH);

    buffer = new Texture(FW, FH, 0,1,GL_RGB8);
    pass = new Texture(FW, FH, 0,1, GL_RGB8);
    FTbuffer = new Texture(FW, FH, 0,1, GL_RGB8,GL_RGB);

    black.resize((mat[0][0]*FW)*(mat[0][1]*FH)*3);
    memset(&black[0],0,mat[0][0]*FW*mat[0][1]*FH*3);

    fb = new FrameBuffer(buffer); 
        
    artnet->universes[uni].remaps.push_back({chan, quantity, (float*)&engine.dynamic_ubo.buffer.data[fix1UBO->offset] });
    auto &remap = artnet->universes[uni].remaps.back();

    for (auto &c:fix1UBO->components) { 

        for (auto &m :c->members) { 

            uint8_t combining = 0;

            bool breaker = false;
            for (auto &p:fixture.presets) { if (breaker) { break; }
                for (auto &f:p.features) { if (breaker) { break; }
                    for (auto &a:f.attributes) { 
                      if (&m == a.member) {
                          combining = a.combining; 
                          breaker = true;
                          break;

                        }
                    }
                }
            }

            remap.attributes.push_back({combining, m.range_from, m.range_to});  
        
        }
    }
    
    // artnet->universes[uni].callbacks.push_back([this](DMX* dmx){ 

    //     const char* chars =  "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!?.";

    //     for (int i = 0; i < this->quantity; i++) { 

    //         int gobo_id = *(8+&fix1UBO->buffer->data[fix1UBO->offset+this->attroffset]+(i*dmx->remaps[0].attributes.size()))*255;

    //         if (gobo_id == 10) {

    //             float *l = &matriceUBO->data[i*4+this->matoffset];
    //             int char_id = *(9+&fixtureUBO->data[this->attroffset]+(i*dmx->remaps[0].attributes.size()))*(strlen(chars)-1);

    //             FT fr((chars+char_id), (this->height/this->quantity_y)*.9);

    //             GLuint offset_x = this->width**(l+2)+(((this->width/this->quantity_x)-fr.width)*.5);

    //             this->FTbuffer->write(&black[0], l[0]*this->buffer->width,l[1]*this->buffer->height,this->buffer->width*l[2],this->buffer->height*l[3],0,1,GL_R8,GL_RED); 
    //             this->FTbuffer->write(fr.buffer, fr.width, fr.height,offset_x,this->height**(l+3),0,1,GL_R8,GL_RED); 



    //         }
            

    //      }  
        
    // });

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



        // ///// TEXTURES

        // ImGui::Begin("VIEW");

        // for (int i = 0; i < Texture::pool.size(); i++) {

        //     std::string wh = std::to_string(Texture::pool[i]->width) + " x " + std::to_string(Texture::pool[i]->height);
        //     ImGui::Text(wh.c_str());
            
        //     float ratio = Texture::pool[i]->height/(float)Texture::pool[i]->width;
        //     auto nw = std::min(Texture::pool[i]->width,(GLuint)512);

        //     ImGui::Image((void*)(intptr_t)(ImTextureID)(uintptr_t)Texture::pool[i]->id, ImVec2(nw,nw*ratio));

        //     ImGui::PushID(i+100);

        //     ImGui::PopID();
        //     ImGui::Separator();
        // }
        
        // ImGui::End(); 


        //////////////////////////////////////////////
        //////////////////////////////////////////////
        //////////////////////////////////////////////
        // DEBUG STUFF
        //////////////////////////////////////////////
        //////////////////////////////////////////////

        // if (debug) { 

        //     survey_count = 0;

        //     survey((REPO_DIR+"assets/shader/"+std::string(shader->paths[0])).c_str(), [&](){ shader->reset(); atlas->link(shader); matriceUBO->upload(); }); 
        //     survey((REPO_DIR+"assets/shader/"+std::string(shader->paths[1])).c_str(), [&](){ shader->reset(); atlas->link(shader); matriceUBO->upload(); }); 

        // }
        
//     }); 

// }

// SmartMap& SmartMap::getInstance() { static SmartMap instance;  return instance; }
