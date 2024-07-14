#include "save.hpp"

#include "engine.hpp"
#include "node.hpp"
#include "atlas.hpp"
#include "file.hpp"
#include "effector.hpp"
#include "layer.hpp"
#include "ndi.hpp"


void Save::medias(){


}

void Save::inputs(){



}

void Save::outputs(){


    json_v.document["outputs"].SetObject();
    json_v.document["outputs"].RemoveAllMembers();
    for (auto output : engine.outputs->childrens) {

        auto  outputarr = rapidjson::Value(rapidjson::kArrayType);

        auto output_ = (Output*)output->ptr_(); // big leap

        outputarr.PushBack( output_->width, allocator );
        outputarr.PushBack( output_->height, allocator );
        outputarr.PushBack( output_->offset_x, allocator );
        outputarr.PushBack( output_->offset_y, allocator );
        if (output_->fb) {

            Layer* lay = nullptr;

            engine.stack->each<Layer>([&](Node* node, Layer* layer){ if (&layer->fb == output_->fb) lay = layer; });

            outputarr.PushBack( rapidjson::Value(lay->s_->name().c_str(), allocator ), allocator );

        }

        // if ( output->is_a<NDI::Sender>() ) { }

        auto &outputs = json_v.document["outputs"];

        if ( output->is_a_nowarning<Window>() ) {

            if (!outputs.HasMember("monitor")) outputs.AddMember(rapidjson::Value("monitor", allocator) , rapidjson::Value(rapidjson::kObjectType), allocator);

            outputs["monitor"].AddMember( rapidjson::Value(output->name().c_str(), allocator)  , outputarr, allocator );

        }
        if ( output->is_a_nowarning<NDI::Sender>() ) {

            if (!outputs.HasMember("ndi")) outputs.AddMember(rapidjson::Value("ndi", allocator) , rapidjson::Value(rapidjson::kObjectType), allocator);

            outputs["ndi"].AddMember( rapidjson::Value(output->name().c_str(), allocator)  , outputarr, allocator );

        }



    }

}

void Save::uberlayers(){

    json_v.document["uberlayers"].RemoveAllMembers();

    engine.stack->each<UberLayer>([&](Node* node, UberLayer* ubl){

        auto &arr = json_v.document["uberlayers"];

        auto  ubl_ = rapidjson::Value(rapidjson::kObjectType);

        for (auto layer : ubl->layers) {

            auto  layer_ = rapidjson::Value(rapidjson::kArrayType);

            layer_.PushBack(layer.get()->w, allocator);
            layer_.PushBack(layer.get()->h, allocator);
            layer_.PushBack(layer.get()->s_->quantity(), allocator);

            auto  effectors_ = rapidjson::Value(rapidjson::kObjectType);
            // for (auto e : layer->effectors) effectors_.AddMember( rapidjson::Value(e.get()->ref().name().c_str(), allocator), rapidjson::Value(e.get()->file->filename().c_str(), allocator), allocator );// TODOTODO
            layer_.PushBack(effectors_, allocator);

            ubl_.AddMember(rapidjson::Value(layer.get()->s_->name().c_str(), allocator)  , layer_, allocator);

        }

        arr.AddMember(rapidjson::Value(node->name().c_str(), allocator), ubl_, allocator);

    });

}

void Save::layers(){

   json_v.document["layers"].RemoveAllMembers();

    engine.stack->each<Layer>([&](Node* node, Layer* layer){

        auto &arr = json_v.document["layers"];

        auto  lay = rapidjson::Value(rapidjson::kArrayType);

        lay.PushBack(layer->fb.width, allocator);
        lay.PushBack(layer->fb.height, allocator);

        auto  models = rapidjson::Value(rapidjson::kObjectType);

        for (auto model : layer->models) {

            auto new_model = rapidjson::Value(rapidjson::kArrayType);

            new_model.PushBack(rapidjson::Value(model.get()->file->filename().c_str(), allocator), allocator);

            new_model.PushBack(model.get()->s_->quantity(),allocator);

            auto effects = rapidjson::Value(rapidjson::kObjectType);
            // for (auto e : model.get()->effectors) effects.AddMember( rapidjson::Value(e.get()->ref().name().c_str(), allocator), rapidjson::Value(e.get()->file->filename().c_str(), allocator), allocator ); // TODOTODO
            new_model.PushBack( effects, allocator );

            models.AddMember(rapidjson::Value(model.get()->s_->name().c_str(), allocator), new_model, allocator);

        }

        lay.PushBack(models, allocator);

        auto effects = rapidjson::Value(rapidjson::kObjectType);
        // for (auto e : layer->effectors) effects.AddMember( rapidjson::Value(e.get()->ref().name().c_str(), allocator), rapidjson::Value(e.get()->file->filename().c_str(), allocator), allocator );// TODOTODO
        lay.PushBack( effects, allocator );

        arr.AddMember( rapidjson::Value(node->name().c_str(), allocator)  , lay, allocator );

    });


}

void Save::effectors(){


}

void Save::editors(){


    json_v.document["editors"].Clear();

    for (auto e : engine.gui->editors) {

        auto v = rapidjson::Value(rapidjson::kArrayType);

        v.PushBack(0, json_v.document.GetAllocator());
        v.PushBack(0, json_v.document.GetAllocator());
        v.PushBack(0, json_v.document.GetAllocator());
        v.PushBack(0, json_v.document.GetAllocator());

        if (e->selected) v.PushBack(rapidjson::Value(e->selected->nameSTL().c_str(), json_v.document.GetAllocator()), json_v.document.GetAllocator());
        if (e->locked) v.PushBack(rapidjson::Value(true), json_v.document.GetAllocator());

        auto &x = json_v.document["editors"].PushBack(v, json_v.document.GetAllocator());

    }


}

void Save::models(){
    
    json_v.document["models"].RemoveAllMembers();
    for (auto m : engine.models->childrens) { json_v.document["models"].AddMember(rapidjson::Value(m->is_a<File>()->filename().c_str(), json_v.document.GetAllocator()), rapidjson::Value(&m->is_a<File>()->data[0], json_v.document.GetAllocator()), json_v.document.GetAllocator()); }

    json_v.document["effectors"].RemoveAllMembers();
    for (auto m : engine.effectors->childrens) {

        json_v.document["effectors"].AddMember(rapidjson::Value(m->is_a<File>()->filename().c_str(), json_v.document.GetAllocator()), rapidjson::Value(&m->is_a<File>()->data[0], json_v.document.GetAllocator()), json_v.document.GetAllocator());

    }

}


Save::Save() : allocator(json_v.document.GetAllocator()) {}

void Save::json(std::string path) {


    if (!json_v.document.HasMember("editors")) json_v.document.AddMember("editors", rapidjson::Value(rapidjson::kArrayType), json_v.document.GetAllocator());
    if (!json_v.document.HasMember("models")) json_v.document.AddMember("models", rapidjson::Value(rapidjson::kObjectType), json_v.document.GetAllocator());
    if (!json_v.document.HasMember("effectors")) json_v.document.AddMember("effectors", rapidjson::Value(rapidjson::kObjectType), json_v.document.GetAllocator());
    if (!json_v.document.HasMember("layers")) json_v.document.AddMember("layers", rapidjson::Value(rapidjson::kObjectType), json_v.document.GetAllocator());
    if (!json_v.document.HasMember("uberlayers")) json_v.document.AddMember("uberlayers", rapidjson::Value(rapidjson::kObjectType), json_v.document.GetAllocator());
    if (!json_v.document.HasMember("outputs")) json_v.document.AddMember("outputs", rapidjson::Value(rapidjson::kObjectType), json_v.document.GetAllocator());
    if (!json_v.document.HasMember("remaps")) json_v.document.AddMember("remaps", rapidjson::Value(rapidjson::kObjectType), json_v.document.GetAllocator());
 

    // json_v.document["inputs"].SetObject();
    // json_v.document["inputs"].RemoveAllMembers();
    // for (auto input : inputs->childrens) {

    // }

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


    medias();

    models();

    effectors();
    
    layers();

    uberlayers();
    
    outputs();

    inputs();

    editors();

}