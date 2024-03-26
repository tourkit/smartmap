#include "callbacks.hpp"

#include "node.hpp"
#include "file.hpp"
#include "ubo.hpp"
#include "struct.hpp"
#include "model.hpp"
#include "effector.hpp"
#include "directory.hpp"
#include "drawcall.hpp"
#include "engine.hpp"
#include "artnet.hpp"
#include "ndi.hpp"
#include "atlas.hpp"
#include "json.hpp"
#include "buffer.hpp"

void Callbacks::init() {
    
    // ////////// xxx.HPP 
    
    // NODE<xxx>::oncreate([](Node* node, xxx *x){ });
    // NODE<xxx>::onchange([](Node* node, xxx *x){ });
    // NODE<xxx>::onrun([](Node* node, xxx *x){ });

    // NODE<xxx>::onadd<yyy>([](Node*_this,Node*node){ 

    //     auto s = _this->is_a<xxx>();
    //     auto f = node->is_a<yyy>();

    //     return _this->UntypedNode::add(node);
        
    // });

    // NODE<Folder>::oncreate([](Node* node, Folder *folder){ 




    // });

    ////////// FILE.HPP 
    
    NODE<File>::oncreate([](Node* node, File *file){ node->name = file->name(); });

    ////////// Artnet.HPP 

    NODE<Artnet>::onrun([](Node* node, Artnet *an){ an->run(); 
    static int size = 0;

    if (an->universes.size() != size) {
        size = an->universes.size();
        // node->get<Artnet>()->trigchange();
    }
    });

    NODE<Artnet>::onchange([](Node* node, Artnet *an){ 

        for (auto c :node->childrens) { delete c; };

        for (auto &uni :an->universes) { 

            uni.second->id = uni.first;

            node->addPtr<DMX>(uni.second)->name = "universe "+std::to_string(uni.first);

        }

    });

    ////////// UBO.HPP 

    NODE<UBO>::onrun([](Node* node, UBO *ubo){ ubo->upload(); });
    
    NODE<UBO>::oncreate([](Node* node, UBO *ubo){ node->name = ubo->name(); });

    ////////// STRUCT.HPP 

    NODE<Struct>::oncreate([](Node* node, Struct *s){ node->name = s->name(); });

    ////////// ENGINE.HPP (and Stack)

    NODE<Stack>::onadd<DrawCall>([](Node*_this,Node*node){ 

        node->is_a<DrawCall>()->update();

        return node; // missind add() use case (still ? maybe not untyped)

        // maybe moved stored_type to UntypedNode and use if!=UntypedNode get name and so on

    });

    NODE<Stack>::onadd<Layer>([](Node*_this,Node*node){ 

        node->is_a<Layer>()->update();

        return node;

    });

    NODE<Stack>::onadd<UBO>([](Node*_this,Node*node){ return node; });

    NODE<Stack>::onadd<File>([](Node*_this,Node*node){ return node; });

    ////////// DRAWCALL.HPP 

    NODE<Layer>::onrun([](Node* node, Layer *layer){ layer->draw(); });
    
    NODE<Layer>::onchange([](Node* node, Layer *layer){ layer->update(); });

    NODE<VBO>::onrun([](Node* node, VBO *vbo){ vbo->draw(); });
    
    NODE<DrawCall>::onrun([](Node* node, DrawCall *dc){ dc->draw(); });
    
    NODE<DrawCall>::onchange([](Node* node, DrawCall *dc){ dc->update(); });

    NODE<DrawCall>::onadd<File>([](Node*_this,Node*node){ 

        auto &dc = *_this->is_a<DrawCall>();

        auto &model = dc.vbo.add(node->is_a<File>());
    
        node = _this->addPtr<Model>(&model)->node();
        
        return node;
        
    });
    
    NODE<VBO>::onadd<File>([](Node*_this,Node*node){

        auto vbo = _this->is_a<VBO>();
        auto file = node->is_a<File>();

        return _this->addPtr<Model>(&vbo->add(file))->node();

    });
    
    NODE<Layer>::onadd<File>([](Node*_this,Node*node){

        return _this; // tofix;

        // auto layer = _this->is_a<Layer>();

        // auto file = node->is_a<File>();

        // // Two following lines very similar to Engine::open()

        // layer->vbo.import(file);  

        // auto x = _this->addPtr<Struct>(&layer->vbo.models.back());

        // return x->node(); 
        
    });
    
    ////////// MODEL.HPP 

    NODE<Model>::oncreate([](Node* node, Model *model) { node->name = model->name(); });

    NODE<Model>::onadd<File>([](Node*_this,Node*node){ 
        
        auto model = _this->is_a<Model>();
        auto file = node->is_a<File>();

        return _this->addPtr<Effector>(&model->add(file))->node();

    });

    ////////// Effector.HPP 
    
    NODE<Effector>::oncreate([](Node* node, Effector *effector) { if (effector->file) node->name = effector->file->name(); });

    ////////// Atlas.HPP 

    // NODE<Atlas>::onchange([](Node* node, Atlas *atlas) { atlas->update(); });

    //////// Buffer.HPP 

    NODE<Buffer>::onchange([](Node* node, Buffer *buffer) { PLOGD<<"ooo"; });
    
    ////////// ShaderProgram.HPP 

    // NODE<ShaderProgram>::onchange([](Node* node, ShaderProgram *shader) { });

    ////////// Directory.HPP 

    NODE<Directory>::oncreate([](Node* node, Directory *dir){ node->name = dir->path; });

    ////////// NDI.HPP 

    NODE<NDI::Sender>::oncreate([](Node* node, NDI::Sender *sender){ sender->init(); });
    NODE<NDI::Sender>::onrun([](Node* node, NDI::Sender *sender){ sender->tick(); });


    ////////// JSON.HPP 

    NODE<JSON>::oncreate([](Node* node, JSON *json){ if (json->file) node->name = json->file->name(); });

}