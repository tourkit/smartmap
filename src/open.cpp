#include "open.hpp"
#include "engine.hpp"
#include "atlas.hpp"
#include "json.hpp"
#include "texture.hpp"
#include "file.hpp"
#include "effector.hpp"
#include "layer.hpp"
#include "artnet.hpp"
#include "utils.hpp"
#include "ndi.hpp"

static std::string json_error = "JSON error";



bool isOutput(rapidjson::GenericMember<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>& x) {

    if (!x.name.IsString() || !x.value.IsArray()) return false;

    auto arr = x.value.GetArray();

    if (arr.Size() < 4 || !arr[0].IsInt() || !arr[1].IsInt() || !arr[2].IsInt() || !arr[3].IsInt() ) return false;

    if (arr.Size() > 4 && !arr[4].IsString() ) return false;

    return true;

}

void Open::medias(){

    JSON::if_obj_in("medias", json_v.document, [&](auto &m) {

        if (!m.name.IsString() || !m.value.IsString()) return;
        if (strcmp(m.name.GetString(), "atlas") ) return;

        auto atlas = new Atlas(4096, 4096, m.value.GetString());
    
        engine.medias->addPtr<Atlas>(atlas)->owned = true;

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

            TypedNode<Artnet>* an_ = engine.inputs->addOwnr<Artnet>( arr[0].IsString() ? arr[0].GetString() : "" );

            an_->active(1)->name( x.name.GetString() );

            auto &an = *an_->get();

            if (arr.Size() < 2) continue;
            if (!arr[1].IsObject()) { PLOGW << json_error; continue; }

            for (auto &remap : arr[1].GetObj()) {

                if (!remap.name.IsString() || !remap.value.IsArray()) { PLOGW << json_error; continue; }

                auto arr = remap.value.GetArray();
                int x = arr.Size();

                if (arr.Size() < 3 || !arr[0].IsInt() || !arr[1].IsInt() || !arr[2].IsString()) { PLOGW << json_error; continue; }

                // should find (Layer/Model/Effector aka Effectable) node(aka "w" down there) (then get Member path)

                Instance inst(&engine.dynamic_ubo);

                Node* n = engine.tree->child(arr[2].GetString());

                if (!n) { PLOGW << arr[2].GetString() << " not found"; continue; }
                auto vlayer = n->is_a<UberLayer::VLayer>();
                if (vlayer) 
                    inst = inst.find(&vlayer->s);

                if (inst.def() == inst.buff) { PLOGW << json_error; continue; }

                auto &uni = *an.universe(arr[0].GetInt()).instances[0].get();

                std::vector<DMXRemap::Attribute> attrs;
                if ( arr.Size() > 3 && arr[3].IsArray() ) for (auto &x : arr[3].GetArray()) if (x.IsInt()) attrs.push_back({x.GetInt()});

                int q = 1;
                if ( arr.Size() > 4 && arr[4].IsInt() ) q = arr[4].GetInt();

                DMXRemap* dmxremap = new DMXRemap(&uni, &inst.track(), arr[1].GetInt()-1, attrs, q);

                uni.remaps.push_back( dmxremap );

                auto out = an_->addPtr<DMXRemap>(dmxremap)->name(remap.name.GetString());

                std::string sss =arr[2].GetString() ;
                auto w = engine.tree->child(sss);
                if (!w) { PLOGE <<arr[2].GetString()<< " not found"; return; }
                w->referings.insert( out );

            }

        }

    });

}

void Open::outputs(){

    JSON::if_obj_in("outputs", json_v.document, [&](auto &m) {

        if (!m.name.IsString() || !m.value.IsObject()) { PLOGW << json_error; return; }

        if (!strcmp(m.name.GetString(),"ndi")) for (auto &x : m.value.GetObj()) {

            if (!isOutput(x))  { PLOGW << json_error; return; }

            auto arr = x.value.GetArray();
            if (arr.Size() < 5) { PLOGW << json_error; return; }
            // if (arr.Size() == 4) { PLOGW << json_error; return; }


            Node* layer = nullptr;
            if (arr.Size()>4) layer = engine.stack->child(arr[4].GetString());

            Node* n = engine.outputs->addOwnr<NDI::Sender>( arr[0].GetInt() , arr[1].GetInt(), x.name.GetString(), (layer?&layer->is_a<Layer>()->fb:nullptr))->active(false);

            if (layer) layer->referings.insert( n );

        }

        if (!strcmp(m.name.GetString(),"monitor")) for (auto &x : m.value.GetObj()) {

            if (!isOutput(x))  { PLOGW << json_error; return; }

            auto arr = x.value.GetArray();

            Node* layer = nullptr;
            if (arr.Size()>4) layer = engine.stack->child(arr[4].GetString());
            auto window = engine.outputs->addPtr<Window>( &engine.window );

            window->name(x.name.GetString());
            window->get()->fb = (layer?&layer->is_a<Layer>()->fb:nullptr);

            engine.window.size( arr[0].GetInt() , arr[1].GetInt() );
            engine.window.pos( arr[2].GetInt() , arr[3].GetInt() );

            break; // only one alloweed for nowe

        }

    });


}


static void addEffectors(JSONVal v, Node* node) {

     for (auto effector_def : v) {

        if (!effector_def.name().c_str() || !effector_def.str().c_str()) { PLOGW << v.stringify(); continue; }

        Node* effector_;
        effector_ = engine.effectors->child(effector_def.str());
        if (! effector_)   effector_ = engine.stack->child(effector_def.str());
        if (! effector_)   { PLOGW << "not an FX : " << effector_def.str(); continue; }
        
        auto new_ = node->add(effector_);
        if (new_) new_->name(effector_def.str()); 
     
     }

     if (v.size()) node->update();

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

            TypedNode<Layer>* new_layer = engine.stack->addOwnr<Layer>(width,height,layer_def.name());
            
            for (auto model_def : layer_def[models_id]) {

                Node* model_file = nullptr;

                engine.models->each<File>([&](Node* n, File* file) {

                    if (file->filename() == model_def[0].str()) model_file = n;

                });

                if (!model_file) { PLOGW << "no model " << model_def[0].str(); continue; }

                auto new_model_ = new_layer->add(model_file);
                auto new_model = new_model_->get<Model>();

                new_model->name(model_def.name());

                if (model_def[1].isnum()) new_model->get()->s.quantity(model_def[1].num());

                if (model_def[2].isobj()) addEffectors( model_def[2], new_model->node() );

            }
            
            if (layer_def[models_id+1].isobj()) addEffectors( layer_def[models_id+1], new_layer->node() );

        }else{ // uberlayer

            if (!layer_def.name().data()) continue;

            auto ubl_ = engine.stack->addOwnr<UberLayer>();
            auto &ubl = *ubl_->get();
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
                auto l_ = ubl_->addPtr<UberLayer::VLayer>(&l);
                l_->active(true)->name(vlayer_def.name());
                l.s.quantity(count);

                addEffectors( vlayer_def[models_id], l_->node() );
                
            }

            ubl.calc_matrice();

            engine.stack->trigchange();

            ubl.fb.texture->bind(3);

            ubl.update();         
        
        }
    }
}

void Open::effectors(){

    for (auto x : json_v["effectors"]) {

        if (!x.name_v.length() ) continue;

        if (x.str().length()) {

            File file(x.name_v+".glsl", x.str().c_str());

            engine.effectors->addPtr<Effector>(new FileEffector(file))->owned = true;

            continue;

        }

        if (!x.childrens.size()) continue;

        auto  wrap_ = engine.effectors->addPtr<Effector>(new Wrappy(std::vector<Effector*>{},3,x.name_v));

        wrap_->owned=true;
        
        auto wrap = wrap_->get();

        for (auto sub : x) 

            if (sub.str().c_str()) {

                Node* n = (*engine.effectors)[sub.str()];

                if (n && n->is_a<Effector>())  wrap_->add(n);  

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
            if (e[4].IsString()) n = engine.tree->child(e[4].GetString());
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

        f->onchange([&](Node* n) { engine.open(path.c_str()); });

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