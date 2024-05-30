
#include "layer.hpp"
#include "texture.hpp"



///////// Layer ////

Layer::Layer(uint16_t width, uint16_t height, std::string name)

    : fb((width?width:engine.window.width), (height?height:engine.window.height)), DrawCall(name) {

    feedback = new Texture(fb.width,fb.height,2,1, GL_RGB8);

}

void Layer::draw() {

    if (feedback) { feedback->bind(); }

    fb.bind();
    // engine.render_passes[0].bind();

    DrawCall::draw();

    if (feedback) { return feedback->read(fb.texture); }

}



///////// UBERLAYER ////

UberLayer::UberLayer() : DrawCall("imuber") {

    engine.static_ubo.add(&layer_def);

    glsl_layers = &engine.static_ubo[layer_def.name()].track();

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_tex_size);

    PLOGD << "max tex size : " << max_tex_size;

    static UberLayer::ShaderProgramBuilder builder;

    shader.builder = &builder;

}

void UberLayer::calc_matrice(VBO* vbo_) {

    auto layers = this->layers;

    std::sort( layers.begin(), layers.end(), [](auto a,auto b){ return a.h > b.h; } );

    auto first_of_line = layers.begin();

    int max_line_h = 0;
    int last_x = 0;
    int last_y = 0;

    std::vector<std::vector<std::array<int,5>>> matrice = {{}};

    for (auto it = layers.begin(); it != layers.end(); it++ ) {

        if (it->h > max_line_h) max_line_h = it->h;

        if (last_x+it->w > max_tex_size/2) { // beware /2 just for me

            matrice.resize( matrice.size()+1 );
            last_y += max_line_h;
            max_line_h = 0;
            last_x = 0;

        }else{

            matrice.back().emplace_back( std::array<int,5>{it->w, it->h, last_x, last_y, it->id} );
            last_x += it->w;

        }



    }

    if (!matrice.size() || !matrice.back().size()) return;

    float matrice_height = matrice.back()[0][1]+matrice.back()[0][3];
    float matrice_width = matrice[0].back()[0]+matrice[0].back()[2];

    fb.create( matrice_width, matrice_height );

    glsl_layers->def()->quantity(layers.size());

    auto x = vbo[0].def();
    vbo[0].def()->quantity(0);
    vbo[1].def()->quantity(0);

    for (auto &x : matrice) for (auto &y : x) {

        auto w = y[0] / matrice_width;
        auto h = y[1] / matrice_height;
        auto x_ = y[2] / matrice_width *2-1+w;
        auto y_ = y[3] / matrice_height *2-1+h;

        glsl_layers->eq(y[4]).set<glm::vec4>(glm::vec4(w, h, x_, y_));

        vbo.addQuad(w, h, x_, y_);

    }

    shader.create();

    engine.static_ubo.upload();

}

UberLayer::VLayer& UberLayer::addLayer(int w , int h) { // kinda ctor for VLaye

    layers.emplace_back(w,h,layers.size());
    // calc_matrice(nullptr);
    // PLOGI << glsl_layers->eq(layers.size() - 1).get<std::array<float,4>>();

    return layers.back();

}

void UberLayer::draw() {

    fb.clear();

    DrawCall::draw();

}


UberLayer::ShaderProgramBuilder::ShaderProgramBuilder()  {  }

void UberLayer::ShaderProgramBuilder::build() {

    ShaderProgram::Builder::build();

}

void UberLayer::ShaderProgramBuilder::frag() { ShaderProgram::Builder::frag();

	body_fragment += "\ttic();\n";
	body_fragment += "\targb(layer1.myQuad[OBJ].argb.alpha, layer1.myQuad[OBJ].argb.red, layer1.myQuad[OBJ].argb.green, layer1.myQuad[OBJ].argb.blue);\n";
	body_fragment += "\trectangle(layer1.myQuad[OBJ].rectangle.pos, layer1.myQuad[OBJ].rectangle.size);\n";
	body_fragment += "\ttac();\n";
        }
void UberLayer::ShaderProgramBuilder::vert() { ShaderProgram::Builder::vert();}
