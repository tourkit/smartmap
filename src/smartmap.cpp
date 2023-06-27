
#include "smartmap.hpp"

SmartMap::SmartMap() : 

    artnet("2.0.0.222"),
    window(false,400,300),
    atlas("assets/media/"), // order matter, should happen after window
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

    atlas.link(&shader);

    int MAT_X = 5,MAT_Y = 2;
    auto mat = matrice(MAT_X,MAT_Y);
    UBO matriceUBO(&mat[0], mat.size()*sizeof(RectF), "MatriceUBO"); 
    matriceUBO.link(&shader);
    matriceUBO.send(); 
} 

SmartMap& SmartMap::getInstance() { static SmartMap instance;  return instance; }
SmartMap& sm = SmartMap::getInstance();