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




void Open::medias(){


    for (auto &media : json_v["medias"]) {

        if (!media.name().c_str() || !media.str().c_str()) 
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

            auto inst = Instance(*engine.dynamic_ubo)[&dest->parent()->is_a<UberLayer>()->m][&vlayer->m];

            if (inst.stl.size() == 1) 
                { PLOGW << json_error; continue; }

            std::vector<DMXRemap::Attribute> attrs;
            for (auto &x : remap["patch"])
                if (x.isnum()) 
                    attrs.push_back({(int)x.num(1)});

            auto &an = *n->is_a<Artnet>();
            
            auto &uni = an.universe(remap["universe", true].num(1)-1);

            n->trig(Node::RUN);

            DMXRemap* dmxremap = new DMXRemap(Instance(an).loc(&(uni.m)), inst, remap["channel", true].num(1)-1, attrs, remap["quantity"].num(1));

            dmxremap->src.remaps.push_back( dmxremap );

            auto out = n->childrens[0]->addPtr<DMXRemap>(dmxremap)->name(remap.name());

            dest->referings.insert( out );

        }

    }

}


Open::Output isOutput(JSONVal& output) {

    Open::Output out;

    out.name = output.name();

    auto dim = output["dimensions", true];

    out.rect[0] = dim[0].num(1);
    out.rect[1] = dim[1].num(1);

    out.src = output["source", true].str();
    
    return out;

}

void Open::outputs(){

    auto outputs = json_v["outputs"];

    for (auto &monitor : outputs["monitor"]) {

        auto output = isOutput(monitor);

        auto n = engine.outputs->addPtr<Window>( &engine.window );

        engine.window.size( output.rect[0] , output.rect[1] );
        engine.window.pos(  output.rect[2] , output.rect[3] );

        outputs_src[n] = output;

        break; // only one alloweed for nowe
        
    }

    for (auto &ndi : outputs["ndi"]) {

        auto output = isOutput(ndi);

        Node* n = engine.outputs->addOwnr<NDI::Sender>( output.rect[0], output.rect[1], ndi.name())->active(false);

        outputs_src[n] = output;

        n->active(true);

    }

}


static void addEffectors(JSONVal v, Node* layer) {

    if (!v.isarr()) {
        PLOGW << layer->name() << " a.k.a. \"" <<v.name() << "\" is not an array";
        return;
    }

     for (auto effector_def : v) {

        if (!effector_def.str().c_str()) 
            { PLOGW << v.stringify(); continue; }

        Node* effector_;

        if (effector_def.str() == "feedback") {

            auto lay = layer->is_a_nowarning<Layer>();
            if (!lay) 
                lay = layer->parent()->is_a_nowarning<Layer>();

            if (lay)
                layer->addPtr<EffectorRef>(lay->addEffector(lay->feedback()));
            
            continue;


        }

        effector_ = engine.tree->find(effector_def.str());
        
        if (! effector_) 
            continue; 
        
        auto new_ = layer->add(effector_);
     
    }

    if (v.size()) layer->update();

}

void Open::layers(){
    
    for (auto layer_def : json_v["layers"]) {

        if (!layer_def.name().data()) { PLOGW << layer_def.stringify(); continue; }

        auto dim = layer_def["dimensions"];
        int width; 
        int height;

        if (dim.size() == 2)  { // HARD CHECK FOR TYPE BETWEEN VIRTUALLAYER AND LAYER
         
            width = dim[0].num(); 
            height = dim[1].num();

            if (!width) width = engine.window.width;
            if (!height) height = engine.window.height;

            Node* new_layer = engine.stack->addOwnr<Layer>(width,height,layer_def.name());
            
            for (auto model_def : layer_def["models"]) {

                Node* model_file = nullptr;

                engine.models->each<File>([&](Node* n, File* file) {

                    if (file->filename() == model_def["model"].str()) model_file = n;

                });

                if (!model_file) { PLOGW << "no model " << model_def["model"].str(); continue; }

                auto new_model_ = new_layer->add(model_file);

                new_model_->name(model_def.name());

                new_model_->is_a<Model>()->m.quantity(model_def["quantity"].num(1));

                addEffectors( model_def["effectors"], new_model_ );

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

                auto dim = vlayer_def["dimensions", true];

                width = dim[0].num();
                height = dim[1].num();

                if (!width) width = engine.window.width;
                if (!height) height = engine.window.height;


                auto &l = ubl.addLayer(width,height);
                auto name = vlayer_def.name();
                l.m.name(name);
                auto l_ = ubl_->addPtr<UberLayer::VirtualLayer>(&l);
                l_->active(true);

                addEffectors( vlayer_def["effectors"], l_ );

                l.m.quantity(vlayer_def["quantity", true].num(1));
                
            }

            ubl.calc_matrice();

            engine.stack->trig(Node::CHANGE);

            ubl.update();         
        
        }
    }
}

void Open::effectors(){

    for (auto x : json_v["effectors"]) {

        if (!x.name_v.length() ) continue;

        if (x.str().length()) {

            File file(x.name_v+".glsl", x.str().c_str());

            auto e = engine.effectors->addOwnr<FileEffector>(file);
            e->name(file.name());
            e->owned = true;

            continue;

        }

        if (!x.childrens.size()) continue;

        auto  wrap_ = engine.effectors->addOwnr<Wrappy>(std::vector<Effector*>{},3,x.name_v);

        auto wrap = wrap_->is_a<Wrappy>();

        for (auto sub : x) 

            if (sub.str().c_str()) {

                Node* n = (*engine.effectors)[sub.str()];

                if (n && n->is_a<Effector>())  
                    wrap_->add(n);  

            }

        
    }

}

void Open::editors(){



    if (json_v.document.HasMember("editors") && json_v.document["editors"].IsArray()) for (auto &e : json_v.document["editors"].GetArray()) {

        if (!e.IsArray()) { PLOGW << json_error; continue; }

        auto x = e.GetArray();

        if (e.Size()<5) { PLOGW << json_error; continue; }

        engine.gui->editors.push_back(new EditorWidget());


            Node* n = nullptr;
            if (e[4].IsString()) n = engine.tree->find(e[4].GetString());
            if (n) {

                engine.gui->editors.back()->selected = n;

                engine.gui->editors.back()->locked = true;

            }else PLOGW << "no \"" << e[4].GetString() << "\" found";


            // if (e.Size() > 5 && e[5].IsBool() ) engine.gui->editors.back()->locked = e[5].GetBool();


    }

}

void Open::models(){
    
    for (auto x : json_v["models"]) 
        if (x.name_v.length() && x.str().length()) 
            engine.models->addOwnr<File>(x.name_v, x.str().c_str());

}


void Open::json(std::string path) {
    

    
    engine.reset();

    json_v.load(File(path).data.data());


    if (!json_v.loaded) {

        engine.gui->editors.push_back(new EditorWidget());

        engine.gui->editors.push_back(new EditorWidget());

        engine.gui->editors.back()->selected = engine.debug;

        engine.gui->editors.back()->locked = true;

        auto f = engine.debug->addOwnr<File>(path);

        f->on(Node::CHANGE, [&](Node* n) { engine.open(path.c_str()); });

        f->select();

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
        Node* output = engine.stack->find(x.second.src);
        if (!output)
            continue;
        
        x.first->add(output);

        if (x.second.name.length())
            x.first->name(x.second.name);
    }


    outputs_src.clear();

}