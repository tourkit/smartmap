
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

UberLayer::UberLayer() : Layer(0,0,"imuber"), builder(this) {

    engine.static_ubo.add(&layer_def);

    glsl_layers = &engine.static_ubo[layer_def.name()].track();

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_tex_size);

    PLOGD << "max tex size : " << max_tex_size;

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

        for (int i = 0 ; i < it->s.quantity(); i++) {


            if (it->h > max_line_h) max_line_h = it->h;

            if (last_x+it->w > max_tex_size/2) { // beware /2 just for me

                matrice.resize( matrice.size()+1 );
                last_y += max_line_h;
                max_line_h = 0;
                last_x = 0;

            }else{

                last_x += it->w;

            }

            matrice.back().emplace_back( std::array<int,5>{it->w, it->h, (last_x?last_x-it->w:0), last_y, it->id} );

        }
    }

    if (!matrice.size()) return;
    if (!matrice.back().size()) matrice.resize(matrice.size()-1);

    float matrice_height = matrice.back()[0][1]+matrice.back()[0][3];
    float matrice_width = matrice[0].back()[0]+matrice[0].back()[2];

    fb.create( matrice_width, matrice_height );
    feedback->create( matrice_width, matrice_height );


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

    return layers.back();

}


UberLayer::ShaderProgramBuilder::ShaderProgramBuilder(UberLayer* ubl) : ubl(ubl) {  }

void UberLayer::ShaderProgramBuilder::build() {

    effectors.clear();

    for (auto &layer : ubl->layers)
        for (auto effector : layer.effectors)
            effectors.insert(effector.get()->file);

    ShaderProgram::Builder::build();

}

static std::string print_layer(UberLayer::VLayer &layer) {

    std::set<Effector*> unique;

    for (auto x : layer.effectors)
        unique.insert(x.get());

    std::string body_fragment;

	body_fragment += "\ttic();\n";

    for (auto effector : unique) {

        std::string arg_str;

            auto name = layer.s.name();

            if (layer.s.quantity() > 1) name += "[OBJ]";

        for (auto &arg : Effector::get(effector->file).args) {

            arg_str += lower(name)+"."+effector->ref.name()+"."+arg.second+", ";

        }

        arg_str.resize(arg_str.size()-2);

        body_fragment += "\t"+effector->file->name()+"("+arg_str+");\n";
    }


	body_fragment += "\ttac();\n";
    return body_fragment;

}

void UberLayer::ShaderProgramBuilder::frag() { DrawCall::ShaderProgramBuilder::frag();

    if (ubl->layers.size() == 1) body_fragment += print_layer(ubl->layers[0]);

    else {

        int last_id = 0;

        for (auto &x : ubl->layers) {

            if (last_id) body_fragment += "\n} else ";

            last_id += x.s.quantity();

            body_fragment += "if (OBJ < "+std::to_string(last_id)+" ){\n\n" + print_layer(x); ;

        }

        body_fragment += "\n}\n";

    }

    body_fragment += "\n";

}
void UberLayer::ShaderProgramBuilder::vert() { DrawCall::ShaderProgramBuilder::vert();}
