
#include "smartmap.hpp"

SmartMap::SmartMap() {

    std::cout << "deb" << std::endl;


    // order matters for some
    artnet = new Artnet("2.0.0.222");
    window = new GL::Window(false,400,300);
    window->setPos(2560,290);
    window->setSize(1920,1080);
    MAT_X = 5; 
    MAT_Y = 2;
    FW = window->width*MAT_X;
    FH = window->height*MAT_Y;
    gui = new GUI(window->window);
    atlas = new Atlas("assets/media/"), 
    quadA = new VBO("quad.obj");
    quadB = new VBO("quad.obj");
    tex = new Texture("boy.jpg");
    // passBuf = new Texture(nullptr,FW,FH);
    // passBuf->format = GL_RGBA8;
    basic = new ShaderProgram({"test.frag", "basic.vert"});
    shader = new ShaderProgram({"smartmap.frag", "smartmap.vert"});
    blur_x = new ShaderProgram({"blur_x.comp"});
    blur_y = new ShaderProgram({"blur_y.comp"});



    atlas->link(shader);
}
void SmartMap::createFixtures(int count) {

    fixtures.resize(count);
    
    auto mat = matrice(MAT_X,MAT_Y);
    UBO matriceUBO(&mat[0], mat.size()*sizeof(RectF), "MatriceUBO"); 
    matriceUBO.link(shader);
    matriceUBO.send(); 
    
    int address = 1;

    // for (auto f:fixtures) address += f.DMXpatch(1,address, {
    
    //      "Color.Red",
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

    shader->sendUniform("MatriceUBOSize", MAT_X*MAT_Y);

    std::cout << "fin" << std::endl;
    
} 

SmartMap& SmartMap::getInstance() { static SmartMap instance;  return instance; }
SmartMap& sm = SmartMap::getInstance();