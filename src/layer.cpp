
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




///////// LayerBuilder ////

Layer::ShaderProgramBuilder::ShaderProgramBuilder() : ShaderProgramBuilder(nullptr) { }

Layer::ShaderProgramBuilder::ShaderProgramBuilder(DrawCall* dc) : dc(dc) { build(); }

void Layer::ShaderProgramBuilder::build() {

    stride_count = 0;

    effectors.clear();

    for (auto dc_ : engine.stack->childrens) {

        auto dc = dc_->is_a<Layer>();

        for (auto &model : dc->models) for (auto &effector : model.get()->effectors) effectors.insert(effector->file);

        for (auto &effector : dc->effectors) effectors.insert(effector->file);

    }

    ShaderProgram::Builder::build();

}


void Layer::ShaderProgramBuilder::frag() {

    header_fragment.clear();

    header_fragment += "uniform sampler2D medias;\n\n";
    header_fragment += "uniform sampler2D render_pass;\n\n";

    header_fragment += "in vec2 UV;\n\n";
    header_fragment += "out vec4 COLOR;\n\n";
    header_fragment += "in flat int ID;\n\n";

    header_fragment += "vec2 uv = UV;\n\n";
    header_fragment += "vec4 color = vec4(0);\n\n";

    int model_id = 0;

    for (auto file : effectors)  header_fragment += Effector::source(file)+";\n\n";

    header_fragment += "void tic() { COLOR += color; uv = UV; color = vec4(1); }\n\n";
    header_fragment += "void tac() { COLOR += color; uv = UV; color = vec4(0); }\n\n";

    header_fragment += comment_line;

    body_fragment.clear();

    if (dc) for (auto &model : dc->models) {

        for (int instance = 0; instance < model.get()->s.quantity(); instance++) {

            auto name = model.get()->s.name();

            if (model.get()->s.quantity() > 1) name += "["+std::to_string(instance)+"]";

            body_fragment += "\t// "+name+"\n"; // would love this to be a node name instead // still matters ?
            body_fragment += "\ttic();\n"; // would love this to be a node name instead // still matters ?

            for (auto &effector : model.get()->effectors) {

                std::string arg_str;

                for (auto &arg : Effector::get(effector.get()->file).args) {

                    arg_str += dc->s.name()+"."+lower(name)+"."+effector->ref.name()+"."+arg.second+", ";

                }

                arg_str.resize(arg_str.size()-2);

                body_fragment += "\t"+effector->file->name()+"("+arg_str+");\n";
            }

        }

        body_fragment += "\ttac();\n\n";

        body_fragment += "\n";

        model_id++;
    }

    if (dc) {

        for (auto &effector : dc->effectors) {

            std::string arg_str;

            for (auto &arg : Effector::get(effector.get()->file).args) {

                arg_str += dc->s.name()+"."+effector->ref.name()+"."+arg.second+", ";

            }

            arg_str.resize(arg_str.size()-2);

            body_fragment += "\t"+effector->file->name()+"("+arg_str+");\n";
        }

        if (dc->effectors.size()) body_fragment += "\ttac();\n\n";

    }

}

void Layer::ShaderProgramBuilder::vert() {

    header_vertex.clear();

    header_vertex += "layout (location = 0) in vec2 POSITION;\n";
    header_vertex += "layout (location = 1) in vec2 TEXCOORD;\n";
    header_vertex += "layout (location = 3) in int OBJ;\n\n";

    header_vertex += "out vec2 UV;\n\n";
    header_vertex += "out int ID;\n\n";

    body_vertex.clear();
    body_vertex += "\tUV = TEXCOORD;\n\n";
    body_vertex += "\tUV.y = 1-UV.y;\n\n";
    body_vertex += "\tID = gl_InstanceID;\n\n";
    body_vertex += "\t// vec2 pos = POSITION*layer[ID].size+layer[ID].pos;\n\n";

    body_vertex += "\tgl_Position = vec4(POSITION.x,POSITION.y,0,1);\n\n";

}



///////// UBERLAYER ////

UberLayer::UberLayer() : Layer(0,0,"imuber") {

    vbo.add(&VBO::quad);

    engine.static_ubo.add(&layer_def);

    glsl_layers = &engine.static_ubo[layer_def.name()].track();

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_tex_size);

    PLOGD << "max tex size : " << max_tex_size;

}

void UberLayer::calc_matrice(VBO* vbo) {

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

    for (auto &x : matrice) for (auto &y : x) {

        auto w = y[0] / matrice_width;
        auto h = y[1] / matrice_height;
        glsl_layers->eq(y[4]).set<glm::vec4>(glm::vec4(w, h, y[2] / matrice_width *2-1+w, y[3] / matrice_height *2-1+h));

    }

    shader.create(); // to update shade

    // vbo->clear();
    // vbo->addQuad(1,1,0,0);

    engine.static_ubo.upload();

}

void UberLayer::addLayer(int w , int h) { // kinda ctor for VLaye

    layers.emplace_back(w,h,layers.size());
    calc_matrice(nullptr);
    // PLOGI << glsl_layers->eq(layers.size() - 1).get<std::array<float,4>>();

}
