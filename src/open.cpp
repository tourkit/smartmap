#include "open.hpp"
#include "engine.hpp"
#include "atlas.hpp"
#include "json.hpp"
#include "rapidjson/document.h"
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

    JSON::if_obj_in("medias", json_v.document, [&](auto &m) {

        if (!m.name.IsString() || !m.value.IsString()) 
            return;

        if (strcmp(m.name.GetString(), "atlas") ) 
            return;
    
        Node* atlas_ = engine.medias->addOwnr<Atlas>(4096, 4096, m.value.GetString());

        auto atlas = atlas_->is_a<Atlas>();

        engine.effectors->addPtr<Effector>(&atlas->effector);

        atlas->texture->bind();

    });

}

void Open::inputs(){

    JSON::if_obj_in("inputs", json_v.document, [&](auto &m) {

        if (!m.name.IsString() || !m.value.IsObject()) { PLOGW << json_error; return; }

        if (!strcmp(m.name.GetString(),"artnet")) for (auto &x : m.value.GetObj()) {

            if (!x.name.IsString()) { PLOGW << json_error; continue; }

            if (x.value.IsString()) engine.inputs->addOwnr<Artnet>( x.value.IsString() ? x.value.GetString() : "" )->active(1)->name( x.name.GetString() );

            if (!x.value.IsArray()) { PLOGW << json_error; continue; }

            auto arr = x.value.GetArray();

            if (!arr.Size()) { PLOGW << json_error; continue; }

            Node* an_ = engine.inputs->addOwnr<Artnet>( arr[0].IsString() ? arr[0].GetString() : "" );

            an_->active(1)->name( x.name.GetString() );

            auto &an = *an_->is_a<Artnet>();

            if (arr.Size() < 2) continue;
            if (!arr[1].IsObject()) { PLOGW << json_error; continue; }

            for (auto &remap : arr[1].GetObj()) {

                if (!remap.name.IsString() || !remap.value.IsArray()) { PLOGW << json_error; continue; }

                auto arr = remap.value.GetArray();
                int x = arr.Size();

                if (arr.Size() < 3 || !arr[0].IsInt() || !arr[1].IsInt() || !arr[2].IsString()) { PLOGW << json_error; continue; }

                // should find (Layer/Model/Effector aka Effectable) node(aka "w" down there) (then get Member path)


                Node* n = engine.tree->find(arr[2].GetString());

                if (!n) { PLOGW << arr[2].GetString() << " not found"; continue; }
                
                
                auto vlayer = n->is_a<UberLayer::VirtualLayer>();
                
                if (!vlayer)  
                    continue;

                auto inst = Instance(*engine.dynamic_ubo)[&n->parent()->is_a<UberLayer>()->m][&vlayer->m];

                if (inst.stl.size() == 1) { PLOGW << json_error; continue; }

                // TODOTODO

                

                std::vector<DMXRemap::Attribute> attrs;
                if ( arr.Size() > 3 && arr[3].IsArray() ) for (auto &x : arr[3].GetArray()) if (x.IsInt()) attrs.push_back({x.GetInt()});

                int q = 1;
                if ( arr.Size() > 4 && arr[4].IsInt() ) q = arr[4].GetInt();
                
                auto &uni = an.universe(arr[0].GetInt()-1);

                an_->trig(Node::RUN);

                DMXRemap* dmxremap = new DMXRemap(Instance(an).loc(&(uni.m)), inst, arr[1].GetInt()-1, attrs, q);

                dmxremap->src.remaps.push_back( dmxremap );

                auto out = an_->childrens[0]->addPtr<DMXRemap>(dmxremap)->name(remap.name.GetString());

                std::string sss =arr[2].GetString() ;
                auto w = engine.tree->find(sss);
                if (!w) { PLOGE <<arr[2].GetString()<< " not found"; return; }
                w->referings.insert( out );

            }

        }

    });

}

struct JSONOutput { int rect[4] = {1,1,0,0}; std::string name, src;};

JSONOutput isOutput(rapidjson::GenericMember<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>& x) {

    JSONOutput out;

    if (!x.name.IsString() || !x.value.IsArray()) { 

        PLOGW << json_error; 

        return out; 
    }

    out.name = x.name.GetString();

    auto arr = x.value.GetArray();

    int rect_count = 0;
    for (auto &x : arr) 
        if (x.IsInt() && rect_count < 4)
            out.rect[rect_count++] = x.GetInt();
        else 
            if (x.IsString()) 
                out.src = x.GetString();
    
    return out;

}

void Open::outputs(){

    JSON::if_obj_in("outputs", json_v.document, [&](auto &m) {

        if (!m.name.IsString() || !m.value.IsObject()) { PLOGW << json_error; return; }

        if (!strcmp(m.name.GetString(),"ndi")) for (auto &x : m.value.GetObj()) {

            auto output = isOutput(x);

            Node* layer = nullptr;
            layer = engine.stack->find(output.src);

            Node* n = engine.outputs->addOwnr<NDI::Sender>( output.rect[0], output.rect[1], x.name.GetString(), (layer?&layer->is_a<Layer>()->fb:nullptr))->active(false);

            if (output.name.length())
                n->name(output.name);

            if (layer) 
                layer->referings.insert( n );

            n->active(true);

        }

        if (!strcmp(m.name.GetString(),"monitor")) for (auto &x : m.value.GetObj()) {

            auto output = isOutput(x);

            auto window = engine.outputs->addPtr<Window>( &engine.window );

            engine.window.size( output.rect[0] , output.rect[1] );
            engine.window.pos(  output.rect[2] , output.rect[3] );


            Node* layer = nullptr;
            layer = engine.stack->find(output.src);


            if (output.name.length())
                window->name(output.name);

            if (layer) 
                window->add(layer);

            break; // only one alloweed for nowe

        }

    });

}


static void addEffectors(JSONVal v, Node* layer) {

    if (!v.isarr()) {
        PLOGW << "not an array";
        return;
    }

     for (auto effector_def : v) {

        if (!effector_def.str().c_str()) 
            { PLOGW << v.stringify(); continue; }

        Node* effector_;

        if (effector_def.str() == "feedback") {

            // auto lay = layer->is_a_nowarning<Layer>();
            // if (!lay) 
            //     lay = layer->parent()->is_a_nowarning<Layer>();

            // if (lay)
            //     layer->addPtr<EffectorRef>(lay->addEffector(lay->feedback()));
            
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

        int width = engine.window.width, height = engine.window.height;

        int models_id = 0;

        if (layer_def.isarr())  {  

            if (layer_def[0].isnum() && layer_def[1].isnum()) {

                width = layer_def[0].num(); height = layer_def[1].num();

                models_id = 2;

            }

            if (!layer_def[models_id].isobj()) { PLOGW << layer_def.stringify(); continue; }

            Node* new_layer = engine.stack->addOwnr<Layer>(width,height,layer_def.name());
            
            for (auto model_def : layer_def[models_id]) {

                Node* model_file = nullptr;

                engine.models->each<File>([&](Node* n, File* file) {

                    if (file->filename() == model_def[0].str()) model_file = n;

                });

                if (!model_file) { PLOGW << "no model " << model_def[0].str(); continue; }

                auto new_model_ = new_layer->add(model_file);

                new_model_->name(model_def.name());

                if (model_def[1].isnum()) 
                    new_model_->is_a<Model>()->m.quantity(model_def[1].num());

                addEffectors( model_def[2], new_model_ );

            }
            
            addEffectors( layer_def[models_id+1], new_layer );

        }else{ // uberlayer

            if (!layer_def.name().data()) continue;

            auto ubl_ = engine.stack->addOwnr<UberLayer>();
            auto &ubl = *ubl_->is_a<UberLayer>();
            ubl_->owned = true;
            ubl_->name(layer_def.name());

            for (auto vlayer_def : layer_def) {

                if (!vlayer_def.name().data()) continue;

                int count = 1;

                if (vlayer_def[0].isnum() && vlayer_def[1].isnum()) {

                    models_id = 2;

                    width = vlayer_def[0].num();
                    height = vlayer_def[1].num();

                    if (vlayer_def[2].isnum()) {

                        models_id = 3;

                        count = vlayer_def[2].num();

                    }

                }

                auto &l = ubl.addLayer(width,height);
                auto name = vlayer_def.name();
                l.m.name(name);
                auto l_ = ubl_->addPtr<UberLayer::VirtualLayer>(&l);
                l_->active(true);
                l.m.quantity(count);

                addEffectors( vlayer_def[models_id], l_ );

                
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
    
    layers();
    
    outputs();

    inputs();

    editors();






}