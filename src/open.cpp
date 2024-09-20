#include "open.hpp"
#include "engine.hpp"
#include "atlas.hpp"
#include "json.hpp"
#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"
#include "texture.hpp"
#include "file.hpp"
#include "effector.hpp"
#include "layer.hpp"
#include "artnet.hpp"
#include "gui.hpp"
#include "ndi.hpp"
#include "editor.hpp"
#include <cctype>
#include <cstdint>

static std::string json_error = "JSON error";

static Node* find(Node* tree, std::string name) {

        
    if (!name.length())
        return nullptr;

    auto ext = split(name,".");
    
    auto x = tree->find(ext[0]);
    
    if (ext.size() == 2 && ext[1].length()) { // supposely a file

        while(x) {
            
            auto file = x->is_a_nowarning<File>();
            if (file && file->extension == ext[1]) 
                return x;

            x = x->find_next();
        }

        if (!x)
            { PLOGE << "no file " << name; }

    }

    return x;

}


void Open::medias(){


    for (auto &media : json_v["medias"]) {

        if (!media.name().length() || !media.str().length()) 
            return;

        if (media.name() != "atlas") // temporary only name allowed
            return;
    
        Node* atlas_ = engine.medias->addOwnr<Atlas>(4096, 4096, media.str());

        auto atlas = atlas_->is_a<Atlas>();

        engine.effectors->addPtr<Effector>(&atlas->effector);

        atlas->texture->bind();

    };

}


void Open::inputs(){

    for (auto &artnet : json_v["inputs"]["artnet"]) {

        Node* n = engine.inputs->addOwnr<Artnet>( artnet["ip", true].str() ) ;

        n->active(1)->name( artnet.name() );

        for (auto &remap : artnet["remaps"]) {

            Node* dest = engine.tree->find(remap["destination", true].str());

            if (!dest) 
                continue; 
            
            auto vlayer = dest->is_a<UberLayer::VirtualLayer>();
            
            if (!vlayer)  
                continue;

            auto xx = dest->parent();
            // if (!xx || !xx->is_a<UberLayer>())
            //     continue;

            auto inst = Instance(*engine.dynamic_ubo)[dest->parent()->is_a<UberLayer>()][vlayer];

            if (inst.stl.size() == 1) 
                { PLOGW << json_error; continue; }

            std::vector<DMXRemap::Attribute> attrs;
            for (auto &x : remap["patch"])
                if (x.isnum()) 
                    attrs.push_back({(int)x.num(1)});

            auto &an = *n->is_a<Artnet>();
            
            auto &uni = an.universe(remap["universe", true].num(1)-1);

            n->trig(Node::RUN);

            DMXRemap* dmxremap = new DMXRemap(Instance(an).loc(&(uni.m)), inst, remap["channel", true].num(1)-1, attrs, remap["quantity|q"].num(1));

            dmxremap->src.remaps.push_back( dmxremap );

            auto out = n->childrens[0]->addPtr<DMXRemap>(dmxremap)->name(remap.name());

            dest->referings.insert( out );

        }

    }

}

void Open::outputs(){

    auto outputs = json_v["outputs"];

    for (auto &monitor : outputs["monitor"]) {

        auto n = engine.tree->find("outputs")->addPtr<Window>( &engine.window )->active(true);

        auto dim = monitor[JSON_DIMENSIONS];
        if (dim[0].num() && dim[1].num())
            engine.window.size( dim[0].num(), dim[1].num());

        auto offset = monitor[JSON_OFFSET];
        engine.window.pos( offset[0].num(), offset[1].num());

        outputs_src.emplace(n,monitor); 

        break; // only one alloweed for nowe
        
    }

    for (auto &ndi : outputs["ndi"]) { 

        auto dim = ndi[JSON_DIMENSIONS];

        Node* n = engine.tree->find("outputs")->addOwnr<NDI::Sender>( dim[0].num(engine.window.width), dim[1].num(engine.window.height), ndi.name());

        outputs_src.emplace(n,ndi);

    }


}

void Open::editors(){

    auto editors = json_v["editors"];

    if (editors.name().length())
        engine.gui(true);
    
    for (auto &e : editors) {

        engine.gui_v->editors.push_back(std::make_shared<EditorWidget>(engine.gui_v));

            Node* n = nullptr;

            if (e[0].str().length()) {

                n = engine.tree->find(e[0].str());
                
                if (!n)      

                    {PLOGW << "no \"" << e[0].str() << "\" found";}

                else {

                    engine.gui_v->editors.back()->selected = n;

                    engine.gui_v->editors.back()->locked = true;
                }

            }   

    }

}

struct JSONModel { 

    std::string name, model; 

    uint32_t width, height;
    int offset_x, offset_y;
    uint32_t q, cols, rows;

    std::vector<std::string> effectors;

    JSONModel(std::string name, std::string model, uint32_t width = 0 , uint32_t height = 0 , int offset_x = 0 , int offset_y = 0 , uint32_t q = 1, uint32_t cols = 1, uint32_t rows = 1) :

        name(name), model(model), width(width), height(height), offset_x(offset_x), offset_y(offset_y), q(q), cols(cols), rows(rows)        
    
    {}
    
    JSONModel(JSONVal& json) {

        name = json.name();

        model = json[JSON_MODELS].str("quad");

        auto dim = json[JSON_DIMENSIONS];
        width = dim[0].num();
        height = dim[1].num();

        auto offset = json[JSON_OFFSET];
        offset_x = offset[0].num();
        offset_y = offset[1].num();

        q = json[JSON_QUANTITY].num(1);

        if (json[JSON_QUANTITY].str().length()) {

            auto grid = split(json[JSON_QUANTITY].str(), "x");

            if ( !grid.size() ||  !is_num(grid[0]))
                return;

            cols = stoi(grid[0]);

            if (grid.size() != 2 || !is_num(grid[1])) {

                q = cols;

                return;

            }

            rows = stoi(grid[1]);

        } 

        if (json[JSON_EFFECTORS].isarr()) 
            for (auto effector : json[JSON_EFFECTORS]) 
                if (effector.str().length())
                    effectors.push_back(effector.str());
    
    };
};



struct JSONLayer { 

    std::string name;
    uint32_t width, height;
    int offset_x, offset_y;
    std::vector<JSONModel> modelsdata; 

    JSONLayer(JSONVal& json) {

        for (auto model : json["models"]) // get models data first
            if (model.name().length()) 
                modelsdata.emplace_back((JSONModel(model)));

        uint32_t width = json[JSON_DIMENSIONS][0].num();
        uint32_t height = json[JSON_DIMENSIONS][1].num();

        if (!width || !height)  // if no dim, need to find from cumulating layers;

            for (auto model : modelsdata) {

                uint32_t m_width = model.width* model.cols+ model.offset_x;
                uint32_t m_height =  model.height* model.rows+ model.offset_y;

                if (m_width > width) width = m_width;
                if (m_height > height) height = m_height;

        }

        name = json.name();
        this->width = width;
        this->height = height;

    }

    void create() {

        if (!modelsdata.size()) // if no models , add engine::quad
            modelsdata.emplace_back("quad", "quad");
        
        auto lay_ = engine.tree->find("main")->addOwnr<Layer>(width, height);

        lay_->name(name);

        for (auto &model : modelsdata) {

            auto file = engine.tree->find(model.model);
            if (!file) {
                PLOGE << " no " << model.model;
                continue;
            }

            auto model_ = lay_->add(file);
            model_->name(model.name);
            
            for (auto effector : model.effectors) {

                auto file = engine.tree->find(effector);
                if (!file) {
                    PLOGE << " no " << effector;
                    continue;
                }

                model_->add(file);
                auto effector_ = model_->childrens.back();

            }

        }

    }

};

static void addFiles (JSONVal val, Node* node) {

    if (!val.name().length())
        return;

        
    if (val.name() == "inputs") return;
    if (val.name() == "editors") return;
    if (val.name() == "outputs") return;
    if (val.name() == "main") return;

    if (val.isobj()){

        auto new_node = node->addOwnr<Node>(val.name())->active(false);

        for (auto x : val) {
        
            addFiles(x, new_node);

        }

    }else if (val.str().length())

        auto new_file = node->addOwnr<File>(val.name(), val.str().c_str())->active(false);

}
void Open::json(std::string path) {
    

    std::vector<JSONLayer> layers_; 

    engine.reset();

    json_v.load(File(path).data.data());


     for (auto x : JSONVal(json_v.document, "doc")) 
        addFiles(x, engine.tree);
    

    for (auto x : json_v["main"]) {

        auto type = x[JSON_TYPE].str();

        if (type == "layer") 
            layers_.push_back(JSONLayer(x));

    }


    if (!json_v.loaded) {

        engine.gui(true);

        engine.gui_v->editors.push_back(std::make_shared<EditorWidget>(engine.gui_v));

        engine.gui_v->editors.push_back(std::make_shared<EditorWidget>(engine.gui_v));

        engine.gui_v->editors.back()->selected = engine.debug;

        engine.gui_v->editors.back()->locked = true;

        auto f = engine.debug->addOwnr<File>(path);

        f->on(Node::CHANGE, [&](Node* n) { engine.open(path.c_str()); });

        engine.gui_v->selected = f;

        return;

    }

    medias();

    outputs();
    
    inputs();


    for (auto &layer_ : layers_) 
        layer_.create();

    for (auto x : outputs_src){ 
        Node* output = engine.main->find(x.second["source"].str());
        if (!output)
            continue;
        
        x.first->add(output);

        if (x.second.name().length())
            x.first->name(x.second.name());
    }


    editors();
    
    outputs_src.clear();

}