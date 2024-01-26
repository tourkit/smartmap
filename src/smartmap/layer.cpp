#include "layer.hpp"  
#include "../smartmap.hpp"  


namespace SmartMap {


Layer::~Layer() {

    delete fb;
    delete quad;
    delete pass;
    delete FTbuffer;

    auto it = std::find(pool.begin(), pool.end(), this);
    if (it != pool.end()) pool.erase(it);

    PLOGD << name;
}



Layer::Layer(uint16_t chan, uint16_t uni, DMX::Fixture &fixture, uint16_t width, uint16_t height, Layer::Mode mode, uint16_t quantity_x, uint16_t quantity_y, float scale, int output, const char* name) 

    : chan(chan), uni(uni), width(width), height(height), mode(mode), quantity_x(quantity_x), quantity_y(quantity_y), quantity(quantity_x*quantity_y),output(output) {

        // stack.list.push_back(new Stack::DrawCall{&Engine::getInstance().quad, sm.shader, nullptr, nullptr, "DC main"});

    fixture_first = Base::fix1UBO->quantity;
    std::vector<char> zeros;
    zeros.resize(quantity*Base::fix1UBO->byte_size);
    Base::fix1UBO->push(&zeros[0],quantity);
    Base::fix2UBO->push(&zeros[0],quantity);
    

    //get attr offset from forin pool
    int first_fixture = 0;
    for (auto &layer:pool) {first_fixture += layer->quantity; for (auto c:Base::fix1UBO->childrens) { attroffset += layer->quantity*((Ptr<Component>*)c)->ptr->members.size(); }}
    // merge the two ? no , c dla merde de faire comme ca tfasson

    GLuint FW = width*scale, FH = height*scale;
    if (mode == Layer::Mode::Free) { FW *= quantity_x; FH *= quantity_y; }
    fb = new FrameBuffer(FW, FH); 

    
    Struct::Entry layerUBO = Base::smartlayersUBO->create();

    // layerUBO.set<uint32_t>(0,fb->id);
    // layerUBO.set<uint32_t>(1,Engine::getInstance().matrices->quantity); // first canva
    // layerUBO.set<uint32_t>(2,(mode==Layer::Mode::Free?quantity:1)); 
    // layerUBO.set<uint32_t>(3,first_fixture); // irst fixture

    // matoffset = Engine::getInstance().matrices->quantity*32;
    // PLOGV << "/!\\ " << matoffset;
    // std::vector<std::array<float, 8>> mats;
    // mats = matrice(quantity_x,quantity_y);
    // for (auto &m:mats) { 

    //     m[6]  = std::min(1.0f,(width*m[0])/(height*m[1]));
    //     m[7]  = std::min(1.0f,(height*m[1])/(width*m[0]));

    // }
    // Engine::getInstance().matrices->push(&mats[0],mats.size());
    // Engine::getInstance().static_ubo.upload(); 


    // id = pool.size();
    // // push to pool
    // pool.push_back(this);
        

    // quad = new VBO("quad.obj", id, "quadSM");

    // pass = new Texture(FW, FH, 0,1, GL_RGB8);
    // FTbuffer = new Texture(FW, FH, 2,1, GL_RGB8,GL_RGB); 

    // black.resize((mats[0][0]*FW)*(mats[0][1]*FH)*3);
    // memset(&black[0],0,mats[0][0]*FW*mats[0][1]*FH*3);

    
    // artnet links 

    // Base::artnet->universes[uni].remaps.push_back({chan, quantity, (float*)&Engine::getInstance().dynamic_ubo.buffer.data[Base::fix1UBO->buffer_offset+attroffset*4] });
    // auto &remap = Base::artnet->universes[uni].remaps.back();

    // for (auto &c:Base::fix1UBO->components) { 

    //     for (auto &m :c->members) { 

    //         uint8_t combining = 0;

    //         bool breaker = false;

    //         for (auto &p:fixture.presets) {
                 
    //             if (breaker) { break; }

    //             for (auto &f:p.features) { 

    //                 if (breaker) { break; }
    //                 for (auto &a:f.attributes) { 

    //                   if (&m == a.member) {

    //                       combining = a.combining; 
    //                       breaker = true;
    //                       break;

    //                     }
    //                 }
    //             }
    //         }

    //         remap.attributes.push_back({combining, m.range_from, m.range_to});  
        
    //     }
    // }
    // // if (pool.size()>1)  return;
    
   

    // int char_h = (this->height/this->quantity_y);
    // float char_widest = 0;
    // for (size_t i = 0; i < strlen(chars)-1; i++) {

    //     FT fr((chars+i), char_h);

    //     if (fr.width>char_widest) char_widest = fr.width;

    // }

    // char_ratio = (fb->width/quantity_x)/char_widest;
    // if (char_ratio > 1) char_ratio = 1;

    // Base::artnet->universes[uni].callbacks.push_back([this](DMX* dmx){ 

    //     for (int i = 0; i < this->quantity; i++) { 

    //         float* ptr = (float*)(Base::fix1UBO->data()+i*Base::fix1UBO->byte_size+attroffset*4)+8;

    //         int gobo_id = (*ptr)*255;

     
    //         if (gobo_id == 10) {


    //             // float* matptr = ((float*)(Engine::getInstance().matrices->data()+this->matoffset))+i*8;

    //             // int char_id = *(ptr+1)*(strlen(this->chars)-1);

    //             // FT fr((this->chars+char_id), (this->height/this->quantity_y)*this->char_ratio);

    //             // float max_width = this->fb->width**(matptr);
                                
    //             // float offset_x = this->fb->width**(matptr+4)+(((max_width)-fr.width)*.5);
    //             // if (offset_x<0) offset_x =0;

    //             // this->FTbuffer->write(&black[0], matptr[0]*this->fb->width,matptr[1]*this->fb->height,this->fb->width*matptr[4],this->fb->height*matptr[5],0,1,GL_R8,GL_RED); 

    //             // this->FTbuffer->write(fr.buffer, fr.width, fr.height, (GLuint)std::floor(offset_x), this->height**(matptr+5),0,1,GL_R8,GL_RED); 



    //         }
            
    //      }  
        
    // });

    if (name) this->name = name;
    else this->name = "SM Layer "+std::to_string(id);

    PLOGD <<  this->name;

};

};

