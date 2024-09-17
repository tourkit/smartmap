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

static std::string json_error = "JSON error";

static Node* find(Node* tree, std::string& name) {

        
    if (!name.length())
        return nullptr;

    auto ext = split(name,".");

    return tree->find(name);


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

        auto n = engine.outputs->addPtr<Window>( &engine.window );

        auto dim = monitor["dimensions|dim|dimension"];
        if (dim[0].num() && dim[1].num())
            engine.window.size( dim[0].num(), dim[1].num());

        auto offset = monitor["offset"];
        engine.window.pos( offset[0].num(), offset[1].num());

        outputs_src.emplace(n,monitor);

        break; // only one alloweed for nowe
        
    }

    for (auto &ndi : outputs["ndi"]) {

        auto dim = ndi["dimensions|dim|dimension"];

        Node* n = engine.outputs->addOwnr<NDI::Sender>( dim[0].num(engine.window.width), dim[1].num(engine.window.height), ndi.name());

        outputs_src.emplace(n,ndi);

    }

}


static void addEffectors(JSONVal v, Node* effectable) {

    if (!v.isarr()) {
        PLOGW << v.name() << " is not an effector";
        return;
    }

     for (auto effector_def : v) {

        if (!effector_def.str().c_str()) 
            { PLOGW << v.stringify(); continue; }

        Node* effector_;

        if (effector_def.str() == "feedback") {

            auto lay = effectable->is_a_nowarning<Layer>();
            if (!lay) 
                lay = effectable->parent()->is_a_nowarning<Layer>();

            if (lay)
                effectable->addPtr<EffectorRef>(lay->addEffector(lay->feedback()));
            
            continue;


        }

        effector_ = engine.tree->find(effector_def.str());
        
        if (! effector_) 
            continue; 
        
        auto new_ = effectable->add(effector_);
     
    }
    
}

void Open::layers(){
    
    for (auto layer_def : json_v["layers"]) {

        if (!layer_def.name().data()) { PLOGW << layer_def.stringify(); continue; }

        if (layer_def["models"].isobj())  { // HARD CHECK FOR TYPE BETWEEN VIRTUALLAYER AND LAYER
         
            auto dim = layer_def["dimensions|dim|dimension"];

            Node* new_layer = engine.stack->addOwnr<Layer>(dim[0].num(),dim[1].num(),layer_def.name());
            
            for (auto model_def : layer_def["models"]) {

                Node* model_file = nullptr;

                engine.models->each<File>([&](Node* n, File* file) {

                    if (file->filename() == model_def["model"].str()) model_file = n;

                });

                if (!model_file) { PLOGW << "no model " << model_def["model"].str(); continue; }

                auto new_model_ = new_layer->add(model_file);

                new_model_->name(model_def.name());

                new_model_->close();

                new_model_->is_a<Model>()->quantity(model_def["quantity|q"].num(1));

                addEffectors( model_def["effectors"], new_model_ );

                // Instance(*engine.dynamic_ubo).print();


            }
            
            if (layer_def["effectors"].size())
                addEffectors( layer_def["effectors"], new_layer );

        }else{ // uberlayer

            auto ubl_ = engine.stack->addOwnr<UberLayer>();
            auto &ubl = *ubl_->is_a<UberLayer>();
            ubl_->owned = true;
            ubl_->name(layer_def.name());

            for (auto vlayer_def : layer_def) {

                if (!vlayer_def.name().data()) 
                    { PLOGW << vlayer_def.stringify(); continue; }

                int count = 1;

                auto dim = vlayer_def["dimensions|dim|dimension"];

                auto &l = ubl.addLayer(dim[0].num(),dim[1].num());
                if (vlayer_def.name().length())
                    l.name(vlayer_def.name());
                auto l_ = ubl_->addPtr<UberLayer::VirtualLayer>(&l);
                l_->active(true);

                addEffectors( vlayer_def["effectors"], l_ );

                l.quantity(vlayer_def["quantity|q"].num(1));
                
            }

            ubl.calc_matrice();

            engine.stack->trig(Node::CHANGE);

            // ubl.updateDC();         
        
        }
    }
}

void Open::effectors(){

    // for (auto x : json_v["effectors"]) {

    //     if (!x.name_v.length() ) continue;

    //     if (x.str().length()) {

    //         File file(x.name_v+".glsl", x.str().c_str());

    //         auto e = engine.effectors->addOwnr<FileEffector>(file);
    //         e->name(file.name());
    //         e->owned = true;

    //         continue;

    //     }

    //     if (!x.childrens.size()) continue;

    //     auto  wrap_ = engine.effectors->addOwnr<Wrappy>(std::vector<Effector*>{},3,x.name_v);

    //     auto wrap = wrap_->is_a<Wrappy>();

    //     for (auto sub : x) 

    //         if (sub.str().c_str()) {

    //             Node* n = (*engine.effectors)[sub.str()];

    //             if (n && n->is_a<Effector>())  
    //                 wrap_->add(n);  

    //         }

        
    // }

}

void Open::editors(){

    auto editors = json_v["editors"];

    if (editors.name().length())
        engine.gui(true);
    
    for (auto &e : editors) {

        engine.gui_v->editors.push_back(new EditorWidget(engine.gui_v));

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

void Open::models(){
    
    for (auto x : json_v["models"]) 
        if (x.name_v.length() && x.str().length()) 
            engine.models->addOwnr<File>(x.name_v, x.str().c_str());

}

static void loop (JSONVal val, Node* node) {

    if (!val.name().length())
        return;

    if (val.isobj()){

        auto new_node = node->addOwnr<Node>(val.name());

        for (auto x : val) 
            loop(x, new_node);

    }else if (val.str().length())

        auto new_file = node->addOwnr<File>(val.name(), val.str().c_str());

}
void Open::json(std::string path) {
    

    
    engine.reset();

    json_v.load(File(path).data.data());
    
    auto xx = JSONVal(json_v.document, "doc");

    for (auto x : xx) {

        if (x.name() == "effectors") continue;
        if (x.name() == "inputs") continue;
        if (x.name() == "models") continue;
        if (x.name() == "editors") continue;
        if (x.name() == "layers") continue;
        if (x.name() == "medias") continue;
        if (x.name() == "outputs") continue;

        loop(x, engine.tree);
        
    }  


    if (!json_v.loaded) {

        engine.gui(true);

        engine.gui_v->editors.push_back(new EditorWidget(engine.gui_v));

        engine.gui_v->editors.push_back(new EditorWidget(engine.gui_v));

        engine.gui_v->editors.back()->selected = engine.debug;

        engine.gui_v->editors.back()->locked = true;

        auto f = engine.debug->addOwnr<File>(path);

        f->on(Node::CHANGE, [&](Node* n) { engine.open(path.c_str()); });

        engine.gui_v->selected = f;

        return;

    }

    medias();

    models();

    effectors();

    outputs();
    
    layers();
    
    inputs();

    editors();




    for (auto x : outputs_src){ 
        Node* output = engine.stack->find(x.second["source"].str());
        if (!output)
            continue;
        
        x.first->add(output);

        if (x.second.name().length())
            x.first->name(x.second.name());
    }


    outputs_src.clear();

}