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
#include "editor.hpp"
#include "window.hpp"

void Save::editors(){

    json_v.document["editors"].Clear();

    for (auto e : engine.gui_v->editors) {

        auto v = rjs::Value(kArrayType);

        if (e->selected) v.PushBack(rjs::Value(e->selected->nameSTL(3).c_str(), json_v.document.GetAllocator()), json_v.document.GetAllocator());

        auto &x = json_v.document["editors"].PushBack(v, json_v.document.GetAllocator());

    }
}

Save::Save() : allocator(json_v.document.GetAllocator()) {}

void Save::saveColor(rjs::Value& value, Node* n) {
    auto &doc = json_v.document;

    if (n->is_active)
        value.AddMember("active", true, doc.GetAllocator());

    if (n->color != std::array<float,4> {1,1,1,1}){
    
        rjs::Value color(kArrayType);
        color.PushBack(n->color[0], doc.GetAllocator());
        color.PushBack(n->color[1], doc.GetAllocator());
        color.PushBack(n->color[2], doc.GetAllocator());
        color.PushBack(n->color[3], doc.GetAllocator());

        if (value.IsObject())
            value.AddMember("color", rjs::Value(color, doc.GetAllocator()), doc.GetAllocator());
    }

}

 void Save::saveFile(JSONVal json, Node* n) {


    auto &doc = json_v.document;
    auto &val = json.value;

    // PLOGW << "create File " << n->name();

    auto file = n->is_a<File>();
    
    json.parent->value.RemoveMember(n->name().c_str()); // apparently gota remove first cant rename directly

    if (n->color != std::array<float,4>{1,1,1,1} || n->is_active) {

        rjs::Value obj(rjs::kObjectType);
        obj.AddMember("source", rjs::Value(file->data.c_str(), doc.GetAllocator()), doc.GetAllocator() );
        obj.AddMember(rjs::Value("type", doc.GetAllocator()), rjs::Value("file", doc.GetAllocator()), doc.GetAllocator() );
        
        saveColor(obj, n);

        json.parent->value.AddMember( rjs::Value(file->filename().c_str(), doc.GetAllocator()), obj, doc.GetAllocator());
        
    }else{

        json.parent->value.AddMember(
            rjs::Value(file->filename().c_str(), doc.GetAllocator()), 
            rjs::Value(file->data.c_str(), doc.GetAllocator()), 
            doc.GetAllocator()
        );
        
    }



}

 void Save::saveWindow(JSONVal json, Node* n) {


    auto &doc = json_v.document;

    // PLOGW << "create Window " << n->name();

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

        rjs::Value offsets(kArrayType);

        offsets.PushBack(window->width, doc.GetAllocator());
        offsets.PushBack(window->height, doc.GetAllocator());

        val.AddMember("offset", offsets, doc.GetAllocator());

    }
    
    saveColor(val, n);

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

void Save::saveRemap(rjs::Value& obj, Node* n) {

    auto &doc = json_v.document;

    auto remap = n->is_a<DMXRemap>();

    obj.AddMember("channel",remap->chan,doc.GetAllocator());

    auto stlname = remap->dst->stl_name(1);

    boost::replace_all(stlname, "dynamic_ubo::", "");

    obj.AddMember("destination",rjs::Value(stlname.c_str(), doc.GetAllocator()),doc.GetAllocator());
    rjs::Value patch_(kArrayType);
    for (auto attr : remap->attributes) {
        
        if (attr.min == 0 && attr.max == 1 && attr.active == true) {// aka default val
        
            patch_.PushBack(attr.combining, doc.GetAllocator());

        }else{

            rjs::Value attr_(kArrayType);

            attr_.PushBack(attr.combining, doc.GetAllocator());
            attr_.PushBack(attr.min, doc.GetAllocator());
            attr_.PushBack(attr.max, doc.GetAllocator());
            attr_.PushBack(attr.active, doc.GetAllocator());

            patch_.PushBack(attr_, doc.GetAllocator());

        }

    }

    if (remap->attributes.size())
        obj.AddMember("patch",patch_,doc.GetAllocator());
    
    saveColor(obj, n);

}

static void saveUniverse(rjs::Document& doc, rjs::Value& obj, Node* n) {


}

 void Save::saveArtnet(JSONVal json, Node* n) {


    auto &doc = json_v.document;

    // PLOGW << "create Artnet " << n->name();

    auto artnet = n->is_a<Artnet>();

    json.value.AddMember("type", "artnet", doc.GetAllocator());

    rjs::Value ip(artnet->ip.c_str(), doc.GetAllocator());
    json.value.AddMember("ip", ip, doc.GetAllocator());

    rjs::Value universes_(kObjectType);

    n->each<Universe>([&](Node* n, Universe* uni) {

        rjs::Value uni_(kObjectType);

        rjs::Value *remaps = &uni_;

        if (n->color != std::array<float,4>{1,1,1,1} || n->is_active) {
            saveColor(uni_, n);
            uni_.AddMember("remaps", rjs::Value(rjs::Value(kObjectType), doc.GetAllocator()), doc.GetAllocator());
            remaps = &uni_["remaps"];
        }
        n->each<DMXRemap>([&](Node* n, DMXRemap* remap) {

            rjs::Value remap_(kObjectType);

            saveRemap(remap_,n);
            
            remaps->AddMember(
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

    saveColor(json.value, n);

    if (n->childrens.size())
        json.value.AddMember(
            rjs::Value("universes", doc.GetAllocator()), 
            rjs::Value(universes_, doc.GetAllocator()), 
            doc.GetAllocator()
        );

}

 void Save::saveEffector(JSONVal json, Node* n) {


    auto &doc = json_v.document;

auto ref = n->is_a<EffectorRef>();

// json.value.setobject(); for mode info like node color


}

 void Save::saveModel(JSONVal json, Node* n) {


    auto &doc = json_v.document;

    // PLOGW << "create Model " << n->name();
    
    auto model = n->is_a<Model>();

    rjs::Value dims(kArrayType);

    // dims.PushBack(model., doc.GetAllocator());
    // dims.PushBack(model., doc.GetAllocator());

    if (model->quantity() != 1)
        json.value.AddMember("quantity", model->quantity(), doc.GetAllocator());

    if (model->effector_refs.size()){

        rjs::Value effectors(kArrayType);

        n->each<EffectorRef>([&](Node* n, EffectorRef* ref) {

            fetch(effectors, n);

        });

        json.value.AddMember("effectors", effectors, doc.GetAllocator());

    }

    saveColor(json.value, n);

}

 void Save::saveLayer(JSONVal json, Node* n) {


    auto &doc = json_v.document;

    // PLOGW << "create Layer " << n->name();

    auto* val_ = &json.value;
    if (val_->HasMember(n->name().c_str()))
        val_ = &(*val_)[n->name().c_str()];
    auto &val = *val_;

    val.AddMember("type", "layer", doc.GetAllocator());
    
    auto layer = n->is_a<Layer>();

    rjs::Value dims(kArrayType);

    dims.PushBack(layer->fb.texture.width, doc.GetAllocator());
    dims.PushBack(layer->fb.texture.height, doc.GetAllocator());

    val.AddMember("resolution", dims, doc.GetAllocator());

    if (layer->quantity() != 1)
        val.AddMember("quantity", layer->quantity(), doc.GetAllocator());

    if (layer->models.size()){
        
        rjs::Value models(rjs::kObjectType);

        n->each<Model>([&](Node* n, Model* m) {

            fetch(models, n);

        });

        val.AddMember("models", models, doc.GetAllocator());

    }
    if (layer->effector_refs.size()){

        rjs::Value effectors(kArrayType);

        n->each<EffectorRef>([&](Node* n, EffectorRef* ref) {

            fetch(effectors, n);

        });

        val.AddMember("effectors", effectors, doc.GetAllocator());

    }

    saveColor(val, n);

}

 void Save::saveNode(JSONVal json, Node* n) {


    auto &doc = json_v.document;
    
    // PLOGW << "create Node " << n->name();

    if (n->color != std::array<float,4>{1,1,1,1} || n->is_active || n->childrens.size()) {

        json.value.AddMember(rjs::Value("childrens", doc.GetAllocator()), rjs::Value(rjs::kObjectType),  doc.GetAllocator());
        for (auto c : n->childrens) 
            fetch(json["childrens"], c);

        saveColor(json.value, n);
        
    }else{

        for (auto c : n->childrens) 
            fetch(json, c);
        
    }

}

 void Save::fetch(JSONVal json, Node* n) {

    auto &doc = json_v.document;

    if (n->hidden) 
        return;
    
    if (n->type()  == typeid(File) && n->is_a<File>() == &VBO::quad) 
        return;

    auto found = json[n->name()];

    if (found.name().length()) {
        found.value.RemoveAllMembers();
        // PLOGW << "found need tpo cleaqr existing " << n->name() ;
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
        
        // PLOGW << "create json entry for " << n->name();
    }

    if (!n->void_ptr || n->is_a_nowarning<Node>())
        saveNode(json[n->name()], n);
    else if (n->is_a_nowarning<File>())
        saveFile(json[n->name()], n);
    else if (n->is_a_nowarning<Layer>())
        saveLayer(json, n);
    else if (n->is_a_nowarning<Model>())
        saveModel(json[n->name()], n);
    else if (n->is_a_nowarning<EffectorRef>())
        saveEffector(json[n->name()], n);
    else if (n->is_a_nowarning<Window>())
        saveWindow(json, n);
    // else if (n->is_a_nowarning<NDI::Sender>())
        // PLOGW << "create NDI " << n->name() << " in " << n->parent()->name();
    else if (n->is_a_nowarning<Artnet>())
        saveArtnet(json[n->name()], n);
    
    else
        PLOGW << "need to create " << n->type_name() << " "  << n->name();

}

void Save::json(std::string path) {

    File file(path);
    json_v.load(&file);

    JSONVal doc(json_v.document, "__JSONVAL__");

    for (auto c : engine.tree->childrens) 
        fetch(doc, c);

    if (!json_v.document.HasMember("editors")) json_v.document.AddMember("editors", rjs::Value(kArrayType), json_v.document.GetAllocator());
    editors();

    rjs::StringBuffer buffer;
    rjs::PrettyWriter<rjs::StringBuffer> writer(buffer);
    writer.SetIndent(' ', 4); // Set indent to 2 spaces
    writer.SetFormatOptions(rjs::kFormatSingleLineArray);
    writer.SetMaxDecimalPlaces(3);
    json_v.document.Accept(writer);

    // inline from depth
    std::string result = std::regex_replace(buffer.GetString(), std::regex(R"(\.0([,\s\)\}\]]))"), "$1");
    // buffer.GetString();std::regex_replace(buffer.GetString(), std::regex(R"(\s{5}(([\]\}])|\s{4,}))"), " $2");
    // result = std::regex_replace(result, std::regex(R"(\n)"), " \n\n");

    // PLOGD << result;
    File::write(path,result);

    PLOGD << "SAVED to " << path;

}