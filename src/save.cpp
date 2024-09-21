#include "save.hpp"

#include <regex>

#include "engine.hpp"
#include "json.hpp"
#include "node.hpp"
#include "atlas.hpp"
#include "artnet.hpp"
#include "file.hpp"
#include "effector.hpp"
#include "layer.hpp"
#include "ndi.hpp"
#include "gui.hpp"
#include "editor.hpp"
#include "rapidjson/rapidjson.h"
#include "remap.hpp"
#include "window.hpp"


void Save::medias(){


}

void Save::inputs(){



}

void Save::outputs(){


    // json_v.document["outputs"].SetObject();
    // json_v.document["outputs"].RemoveAllMembers();
    // for (auto output : engine.outputs->childrens) {

    //     auto  outputarr = rapidjson::Value(rapidjson::kArrayType);

    //     Output* output_ = output->is_a_nowarning<Window>();
    //     if (!output_) output_ = output->is_a_nowarning<NDI::Sender>();
    //     if (!output_) continue;

    //     outputarr.PushBack( output_->width, allocator );
    //     outputarr.PushBack( output_->height, allocator );
    //     outputarr.PushBack( output_->offset_x, allocator );
    //     outputarr.PushBack( output_->offset_y, allocator );
    //     if (output_->fb) {

    //         Layer* lay = nullptr;

    //         engine.main->each<Layer>([&](Node* node, Layer* layer){ if (&layer->fb == output_->fb) lay = layer; });

    //         outputarr.PushBack( rapidjson::Value(lay->name().c_str(), allocator ), allocator );

    //     }

    //     // if ( output->is_a<NDI::Sender>() ) { }

    //     auto &outputs = json_v.document["outputs"];

    //     if ( output->is_a_nowarning<Window>() ) {

    //         if (!outputs.HasMember("monitor")) outputs.AddMember(rapidjson::Value("monitor", allocator) , rapidjson::Value(rapidjson::kObjectType), allocator);

    //         outputs["monitor"].AddMember( rapidjson::Value(output->name().c_str(), allocator)  , outputarr, allocator );

    //     }
    //     if ( output->is_a_nowarning<NDI::Sender>() ) {

    //         if (!outputs.HasMember("ndi")) outputs.AddMember(rapidjson::Value("ndi", allocator) , rapidjson::Value(rapidjson::kObjectType), allocator);

    //         outputs["ndi"].AddMember( rapidjson::Value(output->name().c_str(), allocator)  , outputarr, allocator );

    //     }



    // }

}

void Save::layers(){

//    json_v.document["layers"].RemoveAllMembers();

//     engine.stack->each<Layer>([&](Node* node, Layer* layer){

//         auto &arr = json_v.document["layers"];
        
//         if (dynamic_cast<UberLayer*>(layer)) {

//             auto  ubl_ = rapidjson::Value(rapidjson::kObjectType);


//             node->each<UberLayer::VirtualLayer>([&](Node* node_, UberLayer::VirtualLayer* layer){

//                 auto  layer_ = rapidjson::Value(rapidjson::kArrayType);

//                 layer_.PushBack(layer->w, allocator);
//                 layer_.PushBack(layer->h, allocator);
//                 layer_.PushBack(layer->quantity(), allocator);

//                 auto  effectors_ = rapidjson::Value(rapidjson::kArrayType);

//                 for (auto ref : layer->effector_refs) effectors_.PushBack(  rapidjson::Value(ref->effector->name().c_str(), allocator), allocator );

//                 layer_.PushBack(effectors_, allocator);

//                 ubl_.AddMember(rapidjson::Value(node_->name().c_str(), allocator)  , layer_, allocator);

//             });

//             arr.AddMember(rapidjson::Value(node->name().c_str(), allocator), ubl_, allocator);
                
//             return ;
            
//         }

//         auto  lay = rapidjson::Value(rapidjson::kArrayType);

//         lay.PushBack(layer->fb.texture.width, allocator);
//         lay.PushBack(layer->fb.texture.height, allocator);

//         auto  models = rapidjson::Value(rapidjson::kObjectType);

//         for (auto model : layer->models) {

//             auto new_model = rapidjson::Value(rapidjson::kArrayType);

//             new_model.PushBack(rapidjson::Value(model.get()->file->filename().c_str(), allocator), allocator);

//             new_model.PushBack(model.get()->quantity(),allocator);

//             auto effects = rapidjson::Value(rapidjson::kArrayType);
//             for (auto e : model.get()->effector_refs) {

//                 std::string name = e.get()->name();

//                 std::string value;
//                 auto effector = e.get()->effector;
//                 // if (effector->m.ref()) value = effector->m.ref()->name();
//                 // else 
//                 value = effector->name();
              
//                 effects.PushBack(rapidjson::Value(value.c_str(), allocator), allocator ); // TODOTODO

//             }
//             new_model.PushBack( effects, allocator );

//             models.AddMember(rapidjson::Value(model.get()->name().c_str(), allocator), new_model, allocator);

//         }

//         lay.PushBack(models, allocator);

//         auto effects = rapidjson::Value(rapidjson::kArrayType);
//         for (auto e : layer->effector_refs) 
//             effects.PushBack( rapidjson::Value(e.get()->name().c_str(), allocator), allocator );// TODOTODO
//         lay.PushBack( effects, allocator );

//         arr.AddMember( rapidjson::Value(node->name().c_str(), allocator)  , lay, allocator );

//     });


}


void Save::editors(){


    json_v.document["editors"].Clear();

    for (auto e : engine.gui_v->editors) {

        auto v = rapidjson::Value(rapidjson::kArrayType);

        // v.PushBack(0, json_v.document.GetAllocator());
        // v.PushBack(0, json_v.document.GetAllocator());
        // v.PushBack(0, json_v.document.GetAllocator());
        // v.PushBack(0, json_v.document.GetAllocator());

        if (e->selected) v.PushBack(rapidjson::Value(e->selected->nameSTL().c_str(), json_v.document.GetAllocator()), json_v.document.GetAllocator());
        // if (e->locked) v.PushBack(rapidjson::Value(true), json_v.document.GetAllocator());

        auto &x = json_v.document["editors"].PushBack(v, json_v.document.GetAllocator());

    }


}

void Save::models(){
    
    // json_v.document["models"].RemoveAllMembers();
    // for (auto m : engine.models->childrens) { json_v.document["models"].AddMember(rapidjson::Value(m->is_a<File>()->filename().c_str(), json_v.document.GetAllocator()), rapidjson::Value(&m->is_a<File>()->data[0], json_v.document.GetAllocator()), json_v.document.GetAllocator()); }



}
void Save::effectors(){}

static void saveEffectors(Node* node){

    // json_v.document["effectors"].RemoveAllMembers();

    node->each<Effector>([&](Node* n, Effector* effector) {

    //     auto fe = dynamic_cast<FileEffector*>(effector);

    //     if (fe) json_v.document["effectors"].AddMember(rapidjson::Value(fe->file.name().c_str(), json_v.document.GetAllocator()), rapidjson::Value(&fe->file.data[0], json_v.document.GetAllocator()), json_v.document.GetAllocator());
        
    //     auto wrap = dynamic_cast<Wrappy*>(effector);
        
    //     if (wrap) {
            
    //         auto effects = rapidjson::Value(rapidjson::kArrayType);

    //         for (auto e : wrap->effector_refs) effects.PushBack(rapidjson::Value(e->effector->name().c_str(), json_v.document.GetAllocator()), json_v.document.GetAllocator());

    //         json_v.document["effectors"].AddMember(rapidjson::Value(wrap->Effector::name().c_str(), json_v.document.GetAllocator()), effects, json_v.document.GetAllocator());
            
            
    //     }

    });

}


Save::Save() : allocator(json_v.document.GetAllocator()) {}

static void fetch(JSONVal& json, Node* n);

static void saveFile(JSONVal& json, Node* n) {

    PLOGW << "create File " << n->name();

}

static void saveNode(JSONVal& json, Node* n) {
        
    PLOGW << "create Node " << n->name();

    for (auto c : n->childrens) 
        fetch(json, c);

}
 void fetch(JSONVal& json, Node* n) {

    if (n->hidden) 
        return;
    
    auto found = json[n->name()];

    if (found.name().length()) {

        PLOGW << "found need tpo cleaqr existing " << n->name() ;
    }else {

        PLOGW << "create json entry for " << n->name();
    }

    if (!n->void_ptr || n->is_a_nowarning<Node>())
        saveNode(json, n);
    else if (n->is_a_nowarning<File>())
        saveFile(json, n);
    else if (n->is_a_nowarning<Layer>())
        PLOGW << "create Layer " << n->name() << " in " << n->parent()->name();
    else if (n->is_a_nowarning<Model>())
        PLOGW << "create Model " << n->name() << " in " << n->parent()->name();
    else if (n->is_a_nowarning<EffectorRef>())
        PLOGW << "create Effector " << n->name() << " in " << n->parent()->name();
    else if (n->is_a_nowarning<Window>())
        PLOGW << "create Window " << n->name() << " in " << n->parent()->name();
    else if (n->is_a_nowarning<NDI::Sender>())
        PLOGW << "create NDI " << n->name() << " in " << n->parent()->name();
    else if (n->is_a_nowarning<Layer>())
        PLOGW << "create Layer " << n->name() << " in " << n->parent()->name();
    else if (n->is_a_nowarning<Artnet>())
        PLOGW << "create Artnet " << n->name() << " in " << n->parent()->name();
    
    else
        PLOGW << "need to create " << n->type_name() << " "  << n->name();



}

void Save::json(std::string path) {

    File file(path);
    json_v.load(&file);

    JSONVal doc(json_v.document, "__JSONVAL__");

    for (auto c : engine.tree->childrens) 
        fetch(doc, c);

    if (!json_v.document.HasMember("editors")) json_v.document.AddMember("editors", rapidjson::Value(rapidjson::kArrayType), json_v.document.GetAllocator());
    if (!json_v.document.HasMember("models")) json_v.document.AddMember("models", rapidjson::Value(rapidjson::kObjectType), json_v.document.GetAllocator());
    if (!json_v.document.HasMember("effectors")) json_v.document.AddMember("effectors", rapidjson::Value(rapidjson::kObjectType), json_v.document.GetAllocator());
    if (!json_v.document.HasMember("layers")) json_v.document.AddMember("layers", rapidjson::Value(rapidjson::kObjectType), json_v.document.GetAllocator());
    if (!json_v.document.HasMember("outputs")) json_v.document.AddMember("outputs", rapidjson::Value(rapidjson::kObjectType), json_v.document.GetAllocator());
   
    medias();

    models();

    effectors();
    
    layers();

    outputs();

    inputs();

    editors();

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    writer.SetIndent(' ', 2); // Set indent to 2 spaces
    json_v.document.Accept(writer);

    // inline from depth
    std::string result = std::regex_replace(buffer.GetString(), std::regex(R"(\s{5}(([\]\}])|\s{2,}))"), " $2");
    // result = std::regex_replace(result, std::regex(R"(\n)"), " \n\n");

    // PLOGD << result;
    File::write(path,result);

    PLOGD << "SAVED to " << path;



}