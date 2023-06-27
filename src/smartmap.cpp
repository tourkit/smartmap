
#include "smartmap.hpp"

SmartMap::SmartMap() {
    artnet = new Artnet("2.0.0.222");
    window = new GL::Window(false,400,300);
    gui = new GUI(window->window);
    atlas = new Atlas("assets/media/"), // order matter, should happen after windo;
    quadA = new VBO("quad.obj");
    quadB = new VBO("quad.obj");
    tex = new Texture("boy.jpg");
    basic = new ShaderProgram({"test.frag", "basic.vert"});
    shader = new ShaderProgram({"smartmap.frag", "smartmap.vert"});
    blur_x = new ShaderProgram({"blur_x.comp"});
    blur_y = new ShaderProgram({"blur_y.comp"});
    

    window->setPos(2560,290);
    window->setSize(1920,1080);

    atlas->link(shader);

    int MAT_X = 5,MAT_Y = 2;
    auto mat = matrice(MAT_X,MAT_Y);
    UBO matriceUBO(&mat[0], mat.size()*sizeof(RectF), "MatriceUBO"); 
    matriceUBO.link(shader);
    matriceUBO.send(); 

    // Fixture fixtures[1];
    // int address = 1;


    // for (auto f:fixtures) address += f.DMXpatch(1,address, {
    
    // //     // "Color.Red",
    // //     // "Color.Green",
    // //     // "Color.Blue",
    // //     // "Position.Horizontal",
    // //     // "Position.Vertical",
    // //     // "Gobo.ID",
    // //     // "Gobo.Fx1",
    // //     // "Gobo.Fx2",
    // //     // "Gobo.Fx3",
    // //     // "Strobe",
    // //     // "Feedback",
    // //     // "Orientation",
    // //     // "Focus.Horizontal",
    // //     // "Focus.Vertical"
        
    // });

    UBO fixtureUBO(&Attribute::UBO[0], 24*64, "FixtureUBO"); 
    fixtureUBO.link(shader);
    fixtureUBO.send();
} 

SmartMap& SmartMap::getInstance() { static SmartMap instance;  return instance; }
SmartMap& sm = SmartMap::getInstance();