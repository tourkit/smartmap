#include "nodes.hpp"

#include "node.hpp"
#include "file.hpp"
#include "ubo.hpp"
#include "struct.hpp"
#include "engine.hpp"
#include "model.hpp"
#include "shaderfx.hpp"
#include "directory.hpp"
#include "drawcall.hpp"

//check none have gui in.hpp !!

void Nodes::init() {

    // ////////// xxx.HPP 
    
    // NODE<xxx>::oncreate([](Node* node, xxx *x){ });

    // NODE<xxx>::onadd<yyy>([](Node*_this,Node*node){ 

    //     auto s = ((Ptr<xxx>*)_this)->get();
    //     auto f = ((Ptr<yyy>*)node)->get();

    //     return node;
        
    // });

    ////////// FILE.HPP 

    NODE<File>::oncreate([](Node* node, File *file){ node->name = file->name+"."+file->extension+""; });

    NODE<File>::onrun([](Node* node, File *file){ 

        if (file->hasChanged()) { 

            file->reload(); 

            engine.tree.runCB([file](Node* node){

                if (node->ptr_untyped() == file) PLOGD << node->name <<" is a ref of " << file->name << " . in " << node->parent()->name;

            });
            
        }

     });

    ////////// UBO.HPP 

    NODE<UBO>::onrun([](Node* node, UBO *ubo){ ubo->upload(); });
    
    NODE<UBO>::oncreate([](Node* node, UBO *ubo){ node->name = ubo->name; });

    ////////// STRUCT.HPP 

    NODE<Struct>::oncreate([](Node* node, Struct *s){ node->name = s->name; });

    ////////// ENGINE.HPP (and Stack)

    NODE<Stack>::onadd<DrawCall>([](Node*_this,Node*node){ 

        node->is_a<DrawCall>()->update();

        return _this->UntypedNode::add(node); // missind add() use case (still ? maybe not untyped)

        // maybe moved stored_type to UntypedNode and use if!=UntypedNode get name and so on

    });

    NODE<Stack>::onadd<UBO>([](Node*_this,Node*node){ 

        return _this->UntypedNode::add(node);


    });


    NODE<Stack>::onadd<File>([](Node*_this,Node*node){ 

        return _this->UntypedNode::add(node);


    });

    ////////// DRAWCALL.HPP 

    NODE<DrawCall>::onrun([](Node* node, DrawCall *dc){  dc->run(); });
    
    // NODE<DrawCall>::onadd<Model>([](Node*_this,Node*node){ return _this->UntypedNode::add(node); });

    NODE<DrawCall>::onadd<File>([](Node*_this,Node*node){ 
        
        auto dc = _this->is_a<DrawCall>();
        auto file = node->is_a<File>();
        if (!dc || !file) return node;

        auto model = dc->vbo.import(file);
        
        dc->update();

        // return _this->addPtr<Model>(model);
        
        return _this->UntypedNode::add(new Ptr<Model>(model));

    });

    ////////// MODEL.HPP 

    NODE<Model>::oncreate([](Node* node, Model *model) { node->name = model->file->name; });

    NODE<Model>::onadd<ShaderFX>([](Node*_this,Node*node){ return _this->UntypedNode::add(node); });

    NODE<Model>::onadd<File>([](Node*_this,Node*node){ 
        
        auto model = _this->is_a<Model>();
        auto file = node->is_a<File>();
        if (!model || !file) return node;

        auto bad = new ShaderFX(file); // unowned...

        model->addFX(bad);

        auto dc = _this->parent()->is_a<DrawCall>();
        if (dc) dc->update();
        
        return _this->Node::add(new Ptr<ShaderFX>(bad));

    });
    
    NODE<ShaderFX>::oncreate([](Node* node, ShaderFX *fx) { node->name = fx->file->name; });


    ////////// Directory.HPP 

    NODE<Directory>::oncreate([](Node* node, Directory *dir){ 

        node->name = dir->path;

        for (auto f : dir->list) node->addOwnr<File>(f);
        
    });

}