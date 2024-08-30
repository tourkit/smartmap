
#include "layer.hpp"
#include "texture.hpp"
#include "utils.hpp"
#include "engine.hpp"
#include "instance.hpp"


#include "effector.hpp"
#include <string>

///////// Layer ////

Layer::Layer(uint16_t width, uint16_t height, std::string name)

    : fb((width?width:engine.window.width), (height?height:engine.window.height)), DrawCall(name) {

        static bool init = false;

        if (!init) {

            layer_def.striding(true);

            engine.static_ubo->add(&layer_def);

            glsl_layers =  &(*new Instance(*engine.static_ubo))[&layer_def];

            init = true;

        }


    int xxx = glsl_layers->stl.back().m->quantity();

    vbo.layer_id = glsl_layers->stl.back().m->quantity();

    glsl_layers->stl.back().m->quantity(vbo.layer_id+1);

    glsl_layers->eq(vbo.layer_id).set<std::array<float,2>>({(float)width,(float)height});

    engine.static_ubo->upload();

}

void Layer::update() {

    glsl_layers->eq(vbo.layer_id).set<std::array<float,2>>({(float)fb.width,(float)fb.height});

    engine.static_ubo->upload();

    DrawCall::update();

}

void Layer::draw() {

    if (feedback) 
        feedback->texture.bind(); 

    fb.clear();

    shader.use();

    vbo.draw();

    if (feedback) 
        return feedback->texture.read(&fb.texture); 

}

// FeedbackEffector  ////////////////

void Layer::Feedback::post(Builder* builder) { 


}

bool Layer::Feedback::setup(Builder* builder) { 

    // layer->fb.texture.unit = 2;
    layer->fb.texture.sampler_name = "feedback_pass";
    builder->samplers[3] = &layer->fb.texture;

    ADD_UNIQUE<::Effector*>(builder->effectors_fragment, this);
    
    return true; 
    
}

bool Layer::Feedback::body(Builder* builder, std::string prepend) {  
    
    Effector::body(builder, prepend);
    
    return true; 
    
}


std::string Layer::Feedback::source() {

    std::string current;

    current += "void feedback(float intensity) {\n";
    current += "\tcolor += ( texture( feedback_pass, UV) - .002 ) * intensity;\n",
    current += "}\n";
    return current;

}

std::string UberLayer::Feedback::source() {

    std::string current;

    current += "void feedback(float intensity) { // washington\n";
    current += "int obj = int(OBJ);\n";
    current += "\tcolor += ( texture( feedback_pass, UV) - .002 ) * intensity;\n",
    current += "}\n";
    return current;

}


Layer::Feedback::Feedback(Layer* layer) : 

    Effector("feedback"), 
    texture(layer->fb.width,layer->fb.height,3,1, GL_RGB8), 
    layer(layer) 

    {

    m.add<float>("intensity");
    
}




///////// UBEREFFECTOR //// 

void UberEffector::ubl(UberLayer* ubl) {

    ubl_v = ubl;

    if (!ubl) return;

}

UberEffector::UberEffector(UberLayer* ubl) : Effector(ubl?ubl->m.name():"ubereffector") {  

    this->ubl(ubl);

}

std::string  UberEffector::source() { 

    std::string out;
    std::string name = lower(ubl_v->uberlayer_m.name());
    if (ubl_v->uberlayer_m.quantity()>1) name += "[i]";

    out += "void " + ubl_v->m.name()+ "_effector() {\n\n";

        out +="\tvec4 color_=  vec4(0);\n\n";
        
        out +="\tfor (int i =0; i < "+std::to_string(ubl_v->uberlayer_m.quantity())+"; i++) {\n\n";
        
            out +="\t\tvec2 tuv = uv;\n\n";
        
            out +="\t\ttuv *= static_ubo."+name+".uberLayers.size;\n";
        
            out +="\t\ttuv += static_ubo."+name+".uberLayers.norm;\n\n";
        
            out +="\t\tcolor_ += texture("+ubl_v->fb.texture.sampler_name+", tuv);\n\n";
        
        out +="\t}\n\n";
        
        out +="\tcolor *= color_;\n\n";
    
    out +="};\n";

    return out;

}

bool UberEffector::setup(Builder* builder) { 

    // ubl_v->fb.texture.unit = 2;
    ubl_v->fb.texture.sampler_name = ubl_v->m.name()+"_pass";
    builder->samplers[2] = &ubl_v->fb.texture;

    ADD_UNIQUE<::Effector*>(builder->effectors_fragment, this);

    return true; 
    
}

bool UberEffector::body(Builder* builder, std::string prepend) {  
    
    builder->current_model += "\t"+ubl_v->m.name()+"_effector();\n";
    
    return true; }


///////// UBERLAYER ////

UberLayer::UberLayer() : 
    Layer(0,0,"UberLayer"), 
    builder(this), 
    uberlayer_m(engine.static_ubo->next_name(m.name()))
    {

        // fb.texture.unit = 2;
        // fb.texture.reset();

        uberlayer_m.quantity(0);

    uberlayer_m.add(&uberlayer_def);

    engine.static_ubo->add(&uberlayer_m);

    shader.builder(&builder);

    effector.ubl(this);
    
}

void UberLayer::calc_matrice() {

    auto layers = this->layers;

    std::sort( layers.begin(), layers.end(), [](auto a,auto b){ return a.get()->h > b.get()->h; } );

    auto first_of_line = layers.begin();

    int max_line_h = 0;
    int last_x = 0;
    int last_y = 0;
    int count = 0;

    std::vector<std::vector<std::array<int,5>>> matrice = {{}};

    for (auto it = layers.begin(); it != layers.end(); it++ ) {

        for (int i = 0 ; i < it->get()->m.quantity(); i++) {

            matrice.back().emplace_back( std::array<int,5>{it->get()->w, it->get()->h, last_x, last_y, count} );

            if (it->get()->h > max_line_h) max_line_h = it->get()->h;

            if (last_x+it->get()->w > engine.gl_max_texture_size/2) { // beware /2 just for me

                matrice.resize( matrice.size()+1 );
                last_y += max_line_h;
                max_line_h = 0;
                last_x = 0;

            }else{

                last_x += it->get()->w;

            }

            count++;

        }
    }

    if (!matrice.size()) return;
    if (!matrice.back().size()) matrice.resize(matrice.size()-1);

    float matrice_height = matrice.back()[0][1]+matrice.back()[0][3];
    float matrice_width = matrice[0].back()[0]+matrice[0].back()[2];

    fb.create( matrice_width, matrice_height );
    if (feedback)
        feedback->texture.create( matrice_width, matrice_height );

    uberlayer_m.quantity(count);

    vbo.vertices.quantity(0);
    vbo.indices.quantity(0);

    int z = 0;

    for (auto &x : matrice) for (auto &y : x) {

        auto w = y[0] / matrice_width;
        auto h = y[1] / matrice_height;
        auto x_ = y[2] / matrice_width;
        auto y_ = y[3] / matrice_height;

        auto glsl_uberlayer= Instance(*engine.static_ubo)[&uberlayer_m];

        // PLOGW << z << " - "  << "[ "<< y[0] << " " << y[1] << " " << y[2] << " " << y[3] << " ] - [" << w << " " << h << " " << x_ << " " << y_ << " ]";

        glsl_uberlayer.eq(z).set<float,8>({w, h,x_*2-1,y_*2-1, x_, y_,(float)y[0],(float)y[1]});

        vbo.addQuad(w, h, x_, y_);

        z++;
    }

    ((ShaderProgram*)&shader)->create();


    engine.static_ubo->upload();

}

UberLayer::VirtualLayer& UberLayer::addLayer(int w , int h) {

    layers.emplace_back(std::make_shared<UberLayer::VirtualLayer>(w,h,layers.size()));

    auto &l = *layers.back().get();

    m.add(&l.m);

    return *layers.back().get();

}

UberLayer::ShaderProgramBuilder::ShaderProgramBuilder(UberLayer* ubl) : Layer::ShaderProgramBuilder(ubl), ubl(ubl) {  }

void UberLayer::ShaderProgramBuilder::build() {

    DrawCall::ShaderProgramBuilder::build();

    body_fragment += "\tint obj  = int(OBJ);\n\n";

    std::string ar_str = lower(ubl->uberlayer_m.name())+std::string(ubl->uberlayer_m.quantity()>1?"[obj]":"")+".uberLayers";

    if (ubl->layers.size() == 1) body_fragment += print_layer( *ubl->layers[0].get(), lower(dc->m.name()), "obj", ar_str );

    else {

        int last_id = 0;

        for (auto &x : ubl->layers) {

            if (last_id) body_fragment += "\n} else ";

            auto l = last_id;
            last_id += x.get()->m.quantity();

            body_fragment += "if (obj < "+std::to_string(last_id)+" ){ "+(l?"obj -= "+std::to_string(l)+";":"")+"\n\n" + print_layer( *x.get(), lower(dc->m.name()), "obj", ar_str );

        }

        if (ubl->layers.size()) body_fragment += "\n}\n";

    }

    body_fragment += "\n";

}

