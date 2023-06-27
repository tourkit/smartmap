
#include "smartmap.hpp"

SmartMap::SmartMap() : 

    artnet("2.0.0.222"),
    window(false,400,300),
    quadA("quad.obj"),
    quadB("quad.obj"),
    tex("boy.jpg"),
    basic({"test.frag", "basic.vert"}),
    shader({"smartmap.frag", "smartmap.vert"}),
    blur_x({"blur_x.comp"}),
    blur_y({"blur_y.comp"}),
    gui(window.window)
    
{
    window.setPos(2560,290);
    window.setSize(1920,1080);

    glEnable(GL_CLIP_DISTANCE0);
    glEnable(GL_CLIP_DISTANCE1);
    glEnable(GL_CLIP_DISTANCE2);
    glEnable(GL_CLIP_DISTANCE3);


} 

SmartMap& SmartMap::getInstance() { static SmartMap instance;  return instance; }
SmartMap& sm = SmartMap::getInstance();