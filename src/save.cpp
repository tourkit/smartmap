#include "save.hpp"

#include <regex>
#include <string>

#include "engine.hpp"
#include "json.hpp"
#include "model.hpp"
#include "node.hpp"
#include "atlas.hpp"
#include "artnet.hpp"
#include "file.hpp"
#include "effector.hpp"
#include "layer.hpp"
#include "ndi.hpp"
#include "gui.hpp"
#include "editor.hpp"
#include "rapidjson/document.h"
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

    //     auto  outputarr = rjs::Value(rjs::kArrayType);

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

    //         outputarr.PushBack( rjs::Value(lay->name().c_str(), allocator ), allocator );

    //     }

    //     // if ( output->is_a<NDI::Sender>() ) { }

    //     auto &outputs = json_v.document["outputs"];

    //     if ( output->is_a_nowarning<Window>() ) {

    //         if (!outputs.HasMember("monitor")) outputs.AddMember(rjs::Value("monitor", allocator) , rjs::Value(rjs::kObjectType), allocator);

    //         outputs["monitor"].AddMember( rjs::Value(output->name().c_str(), allocator)  , outputarr, allocator );

    //     }
    //     if ( output->is_a_nowarning<NDI::Sender>() ) {

    //         if (!outputs.HasMember("ndi")) outputs.AddMember(rjs::Value("ndi", allocator) , rjs::Value(rjs::kObjectType), allocator);

    //         outputs["ndi"].AddMember( rjs::Value(output->name().c_str(), allocator)  , outputarr, allocator );

    //     }



    // }

}


using namespace rapidjson;

namespace rjs  =rapidjson;

void Save::editors(){

    json_v.document["editors"].Clear();

    for (auto e : engine.gui_v->editors) {

        auto v = rjs::Value(rjs::kArrayType);

        if (e->selected) v.PushBack(rjs::Value(e->selected->nameSTL().c_str(), json_v.document.GetAllocator()), json_v.document.GetAllocator());

        auto &x = json_v.document["editors"].PushBack(v, json_v.document.GetAllocator());

    }
}

Save::Save() : allocator(json_v.document.GetAllocator()) {}

static void fetch(JSONVal json, Node* n, rjs::Document& doc);

static void saveFile(JSONVal json, Node* n, rjs::Document& doc) {

    PLOGW << "create File " << n->name();

    auto file = n->is_a<File>();
    json.parent->value.RemoveMember(n->name().c_str()); // apparently gota remove first cant rename directly

    if (n->color != std::array<float,4>{1,1,1,1} && n->childrens.size()) {

        rjs::Value obj(rjs::kObjectType);
        obj.AddMember("source", rjs::Value(file->data.c_str(), doc.GetAllocator()), doc.GetAllocator() );
        json.parent->value.AddMember( rjs::Value(file->filename().c_str(), doc.GetAllocator()), obj, doc.GetAllocator());
        
        
    }else{

        json.parent->value.AddMember(
            rjs::Value(file->filename().c_str(), doc.GetAllocator()), 
            rjs::Value(file->data.c_str(), doc.GetAllocator()), 
            doc.GetAllocator()
        );
        
    }



}

static void saveWindow(JSONVal json, Node* n, rjs::Document& doc) {

    PLOGW << "create Window " << n->name();

    auto window = n->is_a<Window>();


    auto* val_ = &json.value;
    if (val_->HasMember(n->name().c_str()))
        val_ = &(*val_)[n->name().c_str()];
    auto &val = *val_;



    val.AddMember(rjs::Value("type", doc.GetAllocator()), rjs::Value("monitor", doc.GetAllocator()), doc.GetAllocator());

    if (window->width && window->height) {

        auto dims = rjs::Value(kArrayType);

        dims.PushBack(window->width, doc.GetAllocator());
        dims.PushBack(window->height, doc.GetAllocator());

        val.AddMember(rjs::Value("resolution", doc.GetAllocator()), dims, doc.GetAllocator());

    }

    if (window->offset_x || window->offset_y) {

        rjs::Value offsets(rjs::kArrayType);

        offsets.PushBack(window->width, doc.GetAllocator());
        offsets.PushBack(window->height, doc.GetAllocator());

        val.AddMember("offset", offsets, doc.GetAllocator());

    }

    if (window->fb) {

        Node* source = nullptr;
        engine.tree->eachBreak<Layer>([&](Node* n, Layer* l) {
            if (&l->fb == window->fb) {
                source = n;
                return Node::Break;
            }
            return Node::Null;
        });

        if (source) 
            val.AddMember("source", rjs::Value(source->name().c_str(), doc.GetAllocator()), doc.GetAllocator());
        
    }

}
#include "artnet.hpp"
static void saveArtnet(JSONVal json, Node* n, rjs::Document& doc) {

    PLOGW << "create Artnet " << n->name();

    auto artnet = n->is_a<Artnet>();

    json.value.AddMember("type", "artnet", doc.GetAllocator());

    rjs::Value ip(artnet->ip.c_str(), doc.GetAllocator());
    json.value.AddMember("ip", ip, doc.GetAllocator());

    rjs::Value universes_(kObjectType);

    n->each<Universe>([&](Node* n, Universe* uni) {

        rjs::Value uni_(kObjectType);

        n->each<DMXRemap>([&](Node* n, DMXRemap* remap) {

            rjs::Value remap_(kObjectType);
            remap_.AddMember("channel",0,doc.GetAllocator());
            remap_.AddMember("destination",0,doc.GetAllocator());
            remap_.AddMember("patch",0,doc.GetAllocator());

            uni_.AddMember(
                rjs::Value(n->name().c_str(),doc.GetAllocator()), 
                rjs::Value(remap_, doc.GetAllocator()), 
                doc.GetAllocator()
            );

        });

        if (n->childrens.size())

            universes_.AddMember(
                rjs::Value(std::to_string(uni->id).c_str(),doc.GetAllocator()), 
                rjs::Value(uni_, doc.GetAllocator()), 
                doc.GetAllocator()
            );

    });

    if (n->childrens.size())
        json.value.AddMember(
            rjs::Value("universes", doc.GetAllocator()), 
            rjs::Value(universes_, doc.GetAllocator()), 
            doc.GetAllocator()
        );

}

static void saveEffector(JSONVal json, Node* n, rjs::Document& doc) {

auto ref = n->is_a<EffectorRef>();

// json.value.setobject(); for mode info like node color


}

static void saveModel(JSONVal json, Node* n, rjs::Document& doc) {

    PLOGW << "create Model " << n->name();
    
    auto model = n->is_a<Model>();

    rjs::Value dims(rjs::kArrayType);

    // dims.PushBack(model., doc.GetAllocator());
    // dims.PushBack(model., doc.GetAllocator());

    if (model->quantity() != 1)
        json.value.AddMember("quantity", model->quantity(), doc.GetAllocator());

    if (model->effector_refs.size()){

        rjs::Value effectors(rjs::kArrayType);

        n->each<EffectorRef>([&](Node* n, EffectorRef* ref) {

            fetch(effectors, n, doc);

        });

        json.value.AddMember("effectors", effectors, doc.GetAllocator());

    }

}

static void saveLayer(JSONVal json, Node* n, rjs::Document& doc) {

    PLOGW << "create Layer " << n->name();

    json.value.AddMember("type", "layer", doc.GetAllocator());
    
    auto layer = n->is_a<Layer>();

    rjs::Value dims(rjs::kArrayType);

    dims.PushBack(layer->fb.texture.width, doc.GetAllocator());
    dims.PushBack(layer->fb.texture.height, doc.GetAllocator());

    json.value.AddMember("resolution", dims, doc.GetAllocator());

    if (layer->quantity() != 1)
        json.value.AddMember("quantity", layer->quantity(), doc.GetAllocator());

    if (layer->models.size()){
        
        rjs::Value models(rjs::kObjectType);

        n->each<Model>([&](Node* n, Model* m) {

            fetch(models, n, doc);

        });

        json.value.AddMember("models", models, doc.GetAllocator());

    }
    if (layer->effector_refs.size()){

        rjs::Value effectors(rjs::kArrayType);

        n->each<EffectorRef>([&](Node* n, EffectorRef* ref) {

            fetch(effectors, n, doc);

        });

        json.value.AddMember("effectors", effectors, doc.GetAllocator());

    }

}

static void saveNode(JSONVal json, Node* n, rjs::Document& doc) {
    
    PLOGW << "create Node " << n->name();

    if (n->color != std::array<float,4>{1,1,1,1} && n->childrens.size()) {

        json.value.AddMember(rjs::Value("childrens", doc.GetAllocator()), rjs::Value(rjs::kObjectType),  doc.GetAllocator());
        for (auto c : n->childrens) 
            fetch(json["childrens"], c, doc);
        
    }else{

        for (auto c : n->childrens) 
            fetch(json, c, doc);
        
    }

}

 void fetch(JSONVal json, Node* n, rjs::Document& doc) {

    if (n->hidden) 
        return;
    
    auto found = json[n->name()];

    if (found.name().length()) {
        found.value.RemoveAllMembers();
        PLOGW << "found need tpo cleaqr existing " << n->name() ;
    }else {

        if (json.value.IsObject())  {
            
        
            if (n->is_a_nowarning<Window>())
                json.value.AddMember(rjs::Value(n->name().c_str(), doc.GetAllocator()), rjs::Value(kObjectType), doc.GetAllocator());
            else
                json.value.AddMember(
                    rjs::Value(n->name().c_str(), doc.GetAllocator()), 
                    rjs::Value(rjs::kObjectType), 
                    doc.GetAllocator()
                );

        }else if (json.value.IsArray())  
            json.value.PushBack(rjs::Value(n->name().c_str(), doc.GetAllocator()), doc.GetAllocator());
        else{
            json.value.SetObject();
            PLOGE << "cant create json entry for " << n->name();
        }
        
        PLOGW << "create json entry for " << n->name();
    }

    if (!n->void_ptr || n->is_a_nowarning<Node>())
        saveNode(json[n->name()], n, doc);
    else if (n->is_a_nowarning<File>())
        saveFile(json[n->name()], n, doc);
    else if (n->is_a_nowarning<Layer>())
        saveLayer(json[n->name()], n, doc);
    else if (n->is_a_nowarning<Model>())
        saveModel(json[n->name()], n, doc);
    else if (n->is_a_nowarning<EffectorRef>())
        saveEffector(json[n->name()], n, doc);
    else if (n->is_a_nowarning<Window>())
        saveWindow(json, n, doc);
    else if (n->is_a_nowarning<NDI::Sender>())
        PLOGW << "create NDI " << n->name() << " in " << n->parent()->name();
    else if (n->is_a_nowarning<Layer>())
        PLOGW << "create Layer " << n->name() << " in " << n->parent()->name();
    else if (n->is_a_nowarning<Artnet>())
        saveArtnet(json[n->name()], n, doc);
        
    
    else
        PLOGW << "need to create " << n->type_name() << " "  << n->name();

    if (n->color != std::array<float,4> {1,1,1,1}){
    
        rjs::Value color(rjs::kArrayType);
        color.PushBack(n->color[0], doc.GetAllocator());
        color.PushBack(n->color[1], doc.GetAllocator());
        color.PushBack(n->color[2], doc.GetAllocator());
        color.PushBack(n->color[3], doc.GetAllocator());

        json[n->name()].value.AddMember("color", color, doc.GetAllocator());
    }
}

void Save::json(std::string path) {

    File file(path);
    json_v.load(&file);

    JSONVal doc(json_v.document, "__JSONVAL__");

    for (auto c : engine.tree->childrens) 
        fetch(doc, c, json_v.document);

    if (!json_v.document.HasMember("editors")) json_v.document.AddMember("editors", rjs::Value(rjs::kArrayType), json_v.document.GetAllocator());
    editors();

    rjs::StringBuffer buffer;
    rjs::PrettyWriter<rjs::StringBuffer> writer(buffer);
    writer.SetIndent(' ', 2); // Set indent to 2 spaces
    json_v.document.Accept(writer);

    // inline from depth
    std::string result = std::regex_replace(buffer.GetString(), std::regex(R"(\s{5}(([\]\}])|\s{2,}))"), " $2");
    // result = std::regex_replace(result, std::regex(R"(\n)"), " \n\n");

    // PLOGD << result;
    File::write(path,result);

    PLOGD << "SAVED to " << path;

}