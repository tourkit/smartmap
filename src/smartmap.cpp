
#include "smartmap.hpp"
#include "stack.hpp"

#include "imgui/imgui.h"

SmartMap::SmartMap() {

    artnet = new Artnet{"2.0.0.222"};
    shader = new ShaderProgram({"smartmap.frag", "smartmap.vert"});
    basicshader = new ShaderProgram({"basic.frag", "smartmap.vert"});
    layershader = new ShaderProgram({"layer.frag", "basic.vert"});

            
    auto &window = Engine::getInstance().window;
    window.setPos(2560-400,0);
    window.setSize(400,300);
  
              
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
        
    }, 24 );

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
            
            layer->quad->draw(layer->quantity); 
            
            layer->pass->read(layer->fb->texture);
            
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

#include "include/vendor/rapidjson/document.h"
#include "include/vendor/rapidjson/stringbuffer.h"

void SmartMap::import(std::string filepath) {


    rapidjson::Document json;
    json.Parse(File(filepath).data.data());
    if(json.HasParseError()) { std::cout << "SM config json parse error !" << std::endl; return; }

    if (!json.HasMember("layers")) { std::cout << "no layers in config" << std::endl; return; }

    for (auto &layer : json["layers"].GetArray()) { 

        SmartMap::Layer::Mode mode = SmartMap::Layer::Mode::Free;
        if (!strcmp(layer["mode"].GetString(),"Grid")) mode = SmartMap::Layer::Mode::Grid;

        int columns = 1, rows = 1;
        if (layer.HasMember("columns")) columns = layer["columns"].GetInt();
        if (layer.HasMember("rows")) rows = layer["rows"].GetInt();
        if (layer.HasMember("quantity")) columns = layer["rows"].GetInt();

        new SmartMap::Layer(

            layer["start_channel"].GetInt(), 
            layer["start_universe"].GetInt(), 
            basic_fixture, 
            engine.window.width,
            engine.window.height, 
            mode, 
            columns,
            rows

        );
    
    }
    
}

SmartMap::Layer::Layer(uint16_t chan, uint16_t uni, DMX::Fixture &fixture, uint16_t width, uint16_t height, Layer::Mode mode, uint16_t quantity_x, uint16_t quantity_y, float scale) 

    : chan(chan), uni(uni), width(width), height(height), mode(mode), quantity_x(quantity_x), quantity_y(quantity_y), quantity(quantity_x*quantity_y) {

        // stack.list.push_back(new Stack::DrawCall{&engine.quad, sm.shader, nullptr, nullptr, "DC main"});

    
    fixture_first = fix1UBO->quantity;
    std::vector<char> zeros;
    zeros.resize(quantity*fix1UBO->byte_size);
    fix1UBO->push(&zeros[0],quantity);
    fix2UBO->push(&zeros[0],quantity);
    

    //get attr offset from forin pool
    int first_fixture = 0;
    for (auto &layer:pool) {first_fixture += layer->quantity; for (auto c:fix1UBO->components) { attroffset += layer->quantity*c->members.size(); }}
    // merge the two ? no , c dla merde de faire comme ca tfasson

    GLuint FW = width*scale, FH = height*scale;
    if (mode == Layer::Mode::Free) { FW *= quantity_x; FH *= quantity_y; }
    fb = new FrameBuffer(FW, FH); 

    
    Buffer::Object::Entry layerUBO = smartlayersUBO->create();

    layerUBO.set<uint32_t>(0,fb->id);
    layerUBO.set<uint32_t>(1,engine.matrices->quantity); // first canva
    layerUBO.set<uint32_t>(2,(mode==Layer::Mode::Free?quantity:1)); 
    layerUBO.set<uint32_t>(3,first_fixture); // irst fixture

    for (auto &layer:pool) { matoffset+=layer->quantity*16; }
    std::vector<std::array<float, 8>> mat;
    mat = matrice(quantity_x,quantity_y);
    engine.matrices->push(&mat[0],mat.size());
    engine.static_ubo.upload(); 


    id = pool.size();
    // push to pool
    pool.push_back(this);
        

    quad = new VBO("quad.obj", id, "quadSM");

    pass = new Texture(FW, FH, 0,1, GL_RGB8);
    // FTbuffer = new Texture(FW, FH, 0,1, GL_RGB8,GL_RGB);

    black.resize((mat[0][0]*FW)*(mat[0][1]*FH)*3);
    memset(&black[0],0,mat[0][0]*FW*mat[0][1]*FH*3);

    
    // artnet links 

    artnet->universes[uni].remaps.push_back({chan, quantity, (float*)&engine.dynamic_ubo.buffer.data[fix1UBO->buffer_offset+attroffset*4] });
    auto &remap = artnet->universes[uni].remaps.back();

    for (auto &c:fix1UBO->components) { 

        for (auto &m :c->members) { 

            uint8_t combining = 0;

            bool breaker = false;

            for (auto &p:fixture.presets) {
                 
                if (breaker) { break; }

                for (auto &f:p.features) { 

                    if (breaker) { break; }

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
    // if (pool.size()>1)  return;
    
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



//     }); 

// }

// SmartMap& SmartMap::getInstance() { static SmartMap instance;  return instance; }
