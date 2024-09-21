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
#include "window.hpp"
#include <cctype>
#include <cstdint>

static std::string json_error = "JSON error";

void Open::medias(){


    // for (auto &media : json_v["medias"]) {

    //     if (!media.name().length() || !media.str().length()) 
    //         return;

    //     if (media.name() != "atlas") // temporary only name allowed
    //         return;
    
    //     Node* atlas_ = engine.medias->addOwnr<Atlas>(4096, 4096, media.str());

    //     auto atlas = atlas_->is_a<Atlas>();

    //     engine.effectors->addPtr<Effector>(&atlas->effector);

    //     atlas->texture->bind();

    // };

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


static void fetch(JSONVal val, Node* node);

static bool operator==(const std::string& a, const char* b) { // kikoo 

    for (auto x : split(b, "|")) 
        if (x == a)
            return true;
     
    return false;

}

static Node* createFile(JSONVal& json, Node* node) {

    auto src = json.str();
    
    if (!src.length())
        src = json[JSON_SOURCE].str();

    if (!src.length())
        return nullptr;
        
    PLOGV << "create File " << json.name() << " in " << node->name();
    
    node = node->addOwnr<File>(json.name(), src.c_str())->active(false);

    return node;
    
}

static Node* createNode(JSONVal& json, Node* node) {

    if (!json.name_v.length())
        return nullptr;

    if (json.name_v != "__JSONVAL__") {

        PLOGV << "create Node " << json.name() << " in " << node->name();
        
        node = node->addOwnr<Node>(json.name_v)->active(false);

    }

    for (auto &c : json[JSON_CHILDRENS]) 
        fetch(c, node); 

    if (!json[JSON_TYPE].str().length()) // only for Node use case metioned later
        for (auto &c : json) 
            fetch(c, node); 


    return node;

}

static Node* createEffector(JSONVal& json, Node* node) {

    PLOGV << "create Effector " << json.name() << " in " << node->name();

    auto file = engine.tree->find(json.str());
    if (!file) {
        PLOGE << " no " << json.str();
        return nullptr;
    }

    node->add(file);

    auto effector_ = node->childrens.back();

    return effector_;

}

static Node* createModel(JSONVal& json, Node* node) {

    PLOGV << "create Model " << json.name() << " in " << node->name();

    auto json_model = json["model"].str();
    if(!json_model.length())
        json_model = "quad";
    auto file = engine.tree->find(json_model);
    if (!file) {
        PLOGE << " no " << json_model << " for " << json.name();
        return  nullptr;
    }

    auto model_ = node->add(file);
    model_->name(json.name());
    
    for (auto effector : json[JSON_EFFECTORS]) 
        createEffector(effector, model_);

    return model_;

}

static std::array<uint32_t,3> getQ(JSONVal& json) {

    uint32_t q=1, cols=1, rows=1;
    
    q = json[JSON_QUANTITY].num(1);

    if (json[JSON_QUANTITY].str().length()) {

        auto grid = split(json[JSON_QUANTITY].str(), "x");

        if ( grid.size() && is_num(grid[0]))
            cols = stoi(grid[0]);

        if (grid.size() == 2 && is_num(grid[1])) {
            q = cols;
        }else
        
            rows = stoi(grid[1]);

    } 

    return {cols*rows, cols, rows};

}
static Node* createArtnet(JSONVal& json, Node* node) {

    PLOGV << "create Artnet " << json.name() << " in " << node->name();

    
        Node* n = node->addOwnr<Artnet>( json["ip", true].str() ) ;

        n->active(1)->name( json.name() );

        for (auto &remap : json["remaps"]) {

            Node* dest = engine.tree->find(remap["destination", true].str());

            if (!dest) 
                continue; 

            auto inst = Instance(*engine.dynamic_ubo)[dest->parent()->is_a<Member>()][dest->is_a_nowarning<Member>()];

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

    
    return node;

}

static Node* createNDI(JSONVal& json, Node* node) {

    PLOGV << "create NDI " << json.name() << " in " << node->name();

    auto dim = json[JSON_DIMENSIONS];

    node->onadd_cb[typeid(Output)] = Node::any_cb;

    Node* n = engine.tree->find("outputs")->addOwnr<NDI::Sender>( dim[0].num(engine.window.width), dim[1].num(engine.window.height), json.name());

    auto src = engine.tree->find(json[JSON_SOURCE].str());
    if (src) 
        node->add(src);


    return node;

}

static Node* createMonitor(JSONVal& json, Node* node) {

    PLOGV << "create Monitor " << json.name() << " in " << node->name();

    Node* already = nullptr;
    node->each<Window>([&](Node* n, Window* w) { already = n; });

    node->onadd_cb[typeid(Output)] = Node::any_cb;

    if (!already)
        node = node->addPtr<Window>( &engine.window )->active(true);

    auto dim = json[JSON_DIMENSIONS];
    if (dim[0].num() && dim[1].num())
        engine.window.size( dim[0].num(), dim[1].num());

    auto offset = json[JSON_OFFSET];
    engine.window.pos( offset[0].num(), offset[1].num());

    auto src = engine.tree->find(json[JSON_SOURCE].str());
    if (src) 
        node->add(src);

    auto parent = node->parent();

    while (parent) {

        parent->active(true);
        parent = parent->parent();
    }

    return node;
}

static Node* createLayer(JSONVal& json, Node* node) {
        
    PLOGV << "create Layer " << json.name() << " in " << node->name();

    auto dim = json[JSON_DIMENSIONS];

    uint32_t width = dim[0].num();
    uint32_t height = dim[1].num();

    if (!width || !height) { // convenient try to find dims in models

        for (auto model : json["models"]) {

            auto dim = model[JSON_DIMENSIONS];
            auto offset = model[JSON_OFFSET];
            auto q = getQ(model);

            uint32_t m_width = dim[0].num()* q[1]+ offset[0].num();
            uint32_t m_height =  dim[1].num()* q[2]+ offset[1].num();

            if (m_width > width) width = m_width;
            if (m_height > height) height = m_height;

        }

    }

    if (!width || !height) { // if still nothing go for window size

        auto &mon = json.owner->find(JSON_TYPE, "monitor");

        if (mon.childrens.size() && !(mon == json_null)) {

                auto dim = mon[JSON_DIMENSIONS];
                width = dim[0].num(1);
                height = dim[1].num(1);
            
        }

    }

    auto lay_ = node->addOwnr<Layer>(width, height);

    lay_->name(json.name());
    lay_->close();
    lay_->active(true);

    for (auto model : json["models"]) 
        createModel(model, lay_);


    auto q = getQ(json);
    if (q[0]!=1)
        lay_->is_a<Layer>()->quantity(q[0]);

    return lay_;
    
}


void fetch(JSONVal json, Node* node) {

    Node* already = nullptr;

    if(json.name_v.length()) {

        already = node->find(json.name_v);

        if(already) {
            node = already;

            for (auto &c : json) 
                fetch(c, already); 
            
            return;

        }
    }

    if (json.isobj()) {

        auto type = lower(json[JSON_TYPE].str());

        if (type.length()) {

            if (type==JSON_NODE) node = createNode(json, node);
            else if (type==JSON_FILE) node = createFile(json, node);
            else if (type==JSON_LAYER) node = createLayer(json, node);
            else if (type==JSON_WINDOW) node = createMonitor(json, node);
            else if (type=="ndi") node = createNDI(json, node);
            else if (type=="artnet") node = createArtnet(json, node);
            else
                PLOGW <<"unknown type \""<< type << " " << (type==JSON_FILE) << " " << (JSON_FILE)<< "\" for " << json.name();   
            
        }else
            node = createNode(json, node);   // special case for Node
        
    }else {

        node = createFile(json, node);

        if (!node)  {  // special case for File
            PLOGV << "WARNING " << json.name();  // shoudnt ever happen
            return;
        }
    }

    if (json[JSON_ACTIVE].name().length())
        node->active(json[JSON_ACTIVE].b());

    auto color = json["color"];
    node->color = {color[0].num(1),color[1].num(1),color[2].num(1),color[3].num(1)};

    // and more default ... (wich could be member names ?)
        
}
void Open::json(std::string path) {


    // engine.reset();

    json_v.load(File(path).data.data());

    // // JSON ERROR WORKSPACE

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

    JSONVal doc(json_v.document, "__JSONVAL__");


    for (auto x : doc) 
        fetch(x, engine.tree);
    



    editors();
    
    outputs_src.clear();

}