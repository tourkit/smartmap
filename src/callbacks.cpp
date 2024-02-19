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

    ////////// FILE.HPP 

    NODE<File>::oncreate([](Node* node, File *file){ if (file->loaded) node->name = file->name+"."+file->extension+""; });

    NODE<File>::onrun([](Node* node, File *file){ 

        if (file->hasChanged()) { 

            PLOGD <<  "file " << file->name << " changed";

            file->reload(); 

            node->update();
   
        }

     });

    ////////// UBO.HPP 

    NODE<UBO>::onrun([](Node* node, UBO *ubo){ ubo->upload(); });
    NODE<UBO>::onchange([](Node* node, UBO *ubo){ ubo->update(); 
    node->updateRefs(node); 
    });
    
    NODE<UBO>::oncreate([](Node* node, UBO *ubo){ node->name = ubo->name; });

    ////////// STRUCT.HPP 

    NODE<Struct>::oncreate([](Node* node, Struct *s){ node->name = s->name; });

    ////////// ENGINE.HPP (and Stack)

    NODE<Stack>::onadd<DrawCall>([](Node*_this,Node*node){ 

        node->is_a<DrawCall>()->update();

        return node; // missind add() use case (still ? maybe not untyped)

        // maybe moved stored_type to UntypedNode and use if!=UntypedNode get name and so on

    });

    NODE<Stack>::onadd<UBO>([](Node*_this,Node*node){ return node; });

    NODE<Stack>::onadd<File>([](Node*_this,Node*node){ return node; });

    ////////// DRAWCALL.HPP 

    NODE<DrawCall>::onrun([](Node* node, DrawCall *dc){  if (dc->shader.loaded) dc->run(); });
    
    NODE<DrawCall>::onchange([](Node* node, DrawCall *dc){ dc->update(); });

    NODE<DrawCall>::onadd<File>([](Node*_this,Node*node){ 
        
        auto dc = _this->is_a<DrawCall>();
        auto file = node->is_a<File>();

        auto model = dc->vbo.import(file);
        
        dc->update();

        auto ptr = new Ptr<Model>(model);

        _this->referings.push_back(ptr->node()); 
        
        return (ptr)->node();

    });

    ////////// MODEL.HPP 

    NODE<Model>::oncreate([](Node* node, Model *model) { node->name = model->file->name; });

    NODE<Model>::onadd<Effector>([](Node*_this,Node*node){ return node; });

    NODE<Model>::onadd<File>([](Node*_this,Node*node){ 
        
        auto model = _this->is_a<Model>();
        auto file = node->is_a<File>();

        auto bad = new Effector(file); // unowned...

        model->addFX(bad);

        auto dc = _this->parent()->is_a<DrawCall>();
        if (dc) {

            dc->update();

            _this->referings.push_back(node);

        }

        return _this;

    });

    ////////// Effector.HPP 
    
    NODE<Effector>::oncreate([](Node* node, Effector *effector) { node->name = effector->file->name; });

    NODE<Effector>::onchange([](Node* node, Effector *effector) { PLOGD<<"update " << effector->file->name; });
    
    ////////// ShaderProgram.HPP 

    // NODE<ShaderProgram>::onchange([](Node* node, ShaderProgram *shader) { });

    ////////// Directory.HPP 

    NODE<Directory>::oncreate([](Node* node, Directory *dir){ 

        node->name = dir->path;

        for (auto f : dir->list) node->addOwnr<File>(f);
        
    });

}