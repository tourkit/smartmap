#include "open.hpp"
#include "engine.hpp"
#include "atlas.hpp"
#include "texture.hpp"
#include "file.hpp"
#include "effector.hpp"
#include "layer.hpp"
#include "artnet.hpp"
#include "utils.hpp"
#include "ndi.hpp"

static std::string json_error = "JSON error";

static void addEffectors(rapidjson::Value &v, Node* node) {

    if (!v.IsObject()) return;

    for (auto &e : v.GetObj()) {

        if (!e.name.IsString() || !e.value.IsString()) continue;

        Node* effector = nullptr;

        engine.effectors->each<Effector>([&](Node* n, Effector* effector_) {

            auto file_ = dynamic_cast<FileEffector*>(effector_);

            if (file_ && file_->file.filename() == e.value.GetString()) effector = n;

            else 
                if (n->name() == e.value.GetString()) effector = n;

        });

        if (effector) 
            node->add(effector)->name(e.name.GetString()); 
        else 
            PLOGE << "no effector: " << e.value.GetString();

    }

}

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

            if (arr.Size() < 2 || !arr[1].IsObject()) { PLOGW << json_error; continue; }

            for (auto &remap : arr[1].GetObj()) {

                if (!remap.name.IsString() || !remap.value.IsArray()) { PLOGW << json_error; continue; }

                auto arr = remap.value.GetArray();
                int x = arr.Size();

                if (arr.Size() < 3 || !arr[0].IsInt() || !arr[1].IsInt() || !arr[2].IsString()) { PLOGW << json_error; continue; }

                // should find (Layer/Model/Effector aka Effectable) node(aka "w" down there) (then get Member path)

                Instance inst(&engine.dynamic_ubo);

                bool continue_ = false;
                for (auto name : split(arr[2].GetString())) {

                        inst = inst[name];

                        if (!inst.def()) {

                            PLOGW << json_error;
                             continue_ = true;
                             break;

                        }
                }
                if (continue_) continue;

                if (inst.def() == inst.buff) { PLOGW << json_error; continue; }

                auto &uni = *an.universe(arr[0].GetInt()).instances[0].get();

                std::vector<DMXRemap::Attribute> attrs;
                if ( arr.Size() > 3 && arr[3].IsArray() ) for (auto &x : arr[3].GetArray()) if (x.IsInt()) attrs.push_back({x.GetInt()});

                int q = 1;
                if ( arr.Size() > 4 && arr[4].IsInt() ) q = arr[4].GetInt();

                DMXRemap* dmxremap = new DMXRemap(&uni, &inst.track(), arr[1].GetInt()-1, attrs, q);

                uni.remaps.push_back( dmxremap );

                auto n = an_->addPtr<DMXRemap>(dmxremap)->name(remap.name.GetString());

                std::string sss =arr[2].GetString() ;
                auto w = engine.tree->child(sss);
                if (!w) { PLOGE <<arr[2].GetString()<< " not found"; return; }
                w->referings.insert( n );

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

void Open::uberlayers(){

   JSON::if_obj_in("uberlayers", json_v.document, [&](auto &l) {


        if (!l.name.IsString() || !l.value.IsObject()) { PLOGW << json_error; return; }

        auto ubl_ = engine.stack->addOwnr<UberLayer>();
        auto &ubl = *ubl_->get();

        ubl_->name(l.name.GetString());

        auto list = l.value.GetObj();

        for (auto &x : list) {

            if (!x.name.IsString() || !x.value.IsArray()) { PLOGW << json_error; continue; }

            auto info = x.value.GetArray();

            int width = engine.window.width, height = engine.window.height;

            int effectors_id = 2;

            int q = 1;

            if (info.Size() > 1 && info[0].IsInt() && info[1].IsInt()) {

                width = info[0].GetInt(); height = info[1].GetInt();

                if (info.Size() > 2 && info[2].IsInt()) {

                    q = info[2].GetInt() ;

                    if (info.Size() > 3) effectors_id = 3;

                }

                auto &l = ubl.addLayer(width,height);

                auto l_ = ubl_->addPtr<UberLayer::VLayer>(&l);
                l_->active(true)->name(x.name.GetString());

                l.s_->quantity(q);

                if (info.Size() > effectors_id && info[effectors_id].IsObject()) {

                    for (auto &e : info[effectors_id].GetObj()) {

                        if (!e.name.IsString() || !e.value.IsString()) { PLOGW <<"weird fx"; continue;}

                        Node* effector = nullptr;


                        engine.effectors->each<File>([&](Node* n, File* f) { if (f->filename() == e.value.GetString()) effector = n; });

                        // if (effector) l_->addPtr<Effector>(l.addEffector(&Effector::get(effector->is_a<File>()))); // TODODOODODOD

                    }

                }

            }

        }

        ubl.calc_matrice();

        engine.stack->trigchange();

        ubl.fb.texture->bind(3);

        ubl.update();

    });

}

void Open::layers(){

    JSON::if_obj_in("layers", json_v.document, [&](auto &l) {

        if (!l.value.IsArray()) { PLOGW << json_error; return; }

        auto info = l.value.GetArray();

        int width = engine.window.width, height = engine.window.height;

        int models_id = 0;

        if (info.Size() > 2) if (info[0].IsInt() && info[1].IsInt()) {

            width = info[0].GetInt(); height = info[1].GetInt();

            if (info[2].IsObject()) models_id = 2;

        }

        TypedNode<Layer>* layer = engine.stack->addOwnr<Layer>(width,height,l.name.IsString() ? l.name.GetString() : "");

        if (info.Size() <= models_id || !info[models_id].IsObject()) { PLOGW << json_error; return; }

        for (auto &model : info[models_id].GetObj()) {

            if (!model.name.IsString() || !model.value.IsArray()) { PLOGW << json_error; return; }

            auto info = model.value.GetArray();

            if (!info.Size() || !info[0].IsString()) { PLOGW << json_error; return; }

            Node* model_file = nullptr;

            for (Node* x : engine.models->childrens) {

                auto f = x->is_a<File>();

                if (!f) { PLOGW << json_error; return; }

                std::string y = info[0].GetString();

                if (f->filename() == y) model_file = x;

            }

            // for (auto x : engine.models->childrens) if (x->is_a<File>() && x->is_a<File>()->filename == info[0].GetString()) model_file = x;

            if (!model_file) { PLOGE << "no model : " << info[0].GetString(); { PLOGW << json_error; return; } }

            auto new_model = layer->add(model_file)->get<Model>();

            new_model->name(model.name.GetString());

            if (info.Size() < 2 || !info[1].IsInt()) { PLOGW << json_error; return; }

            new_model->get()->s_->quantity(info[1].GetInt());

            if (info.Size() < 3 || !info[2].IsObject()) return;

            addEffectors( info[2], new_model->node() );

        }

        addEffectors( info[info.Size()-1], layer->node());

        layer->update();


    });

}

void Open::effectors(){

    JSON::if_obj_in("effectors", json_v.document, [&](auto &m) {

        if (m.name.IsString() && m.value.IsString()) {

            File file(m.name.GetString(), m.value.GetString());
            auto x = new FileEffector(file);
            auto n = engine.effectors->addPtr<Effector>(x);
            n->owned = true;

            return;

        }
        if (m.name.IsString() && m.value.IsArray()) {

            auto arr = m.value.GetArray();

            Wrappy* wrap = new Wrappy({},3,m.name.GetString() );

            for (auto &x : arr) {

                if (!x.IsString()) continue;

                Node* n = (*engine.effectors)[x.GetString()];
                if (!n) continue;
                Effector* effector = n->is_a<Effector>();

                if (effector)  wrap->addEffector(effector);  


            }

            auto wrap_ = engine.effectors->addPtr<Effector>(wrap)->owned = true;


        }
    });

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

    JSON::if_obj_in("models", json_v.document,[&](auto &m) {

        if (m.name.IsString() && m.value.IsString()) 

            auto n = engine.models->addOwnr<File>(m.name.GetString(), m.value.GetString());

    });

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

    uberlayers();
    
    outputs();

    inputs();

    editors();






}