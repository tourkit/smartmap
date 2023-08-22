#ifndef UBO_H
#define UBO_H

#include "pch.hpp"

#include "shader.hpp"
#include "component.hpp"

#include "gui.hpp"

struct UBO {

    static inline std::vector<UBO*> pool;

    std::string name;

    GLuint id;

    GLuint binding;

    std::vector<Shader*> subscribers;

    struct Object {

        std::string name;

        std::vector<Component*> components;

        int quantity;

    };

    std::vector<Object> definition;

    std::vector<float> data;

    ~UBO();
    
    UBO(std::string name, std::vector<Shader*> subscribers = {});

    void destroy();

    void resize();

    void addObject(std::string name, std::vector<Component*> components, size_t quantity = 1);

    void link(GLuint shader);

    void update();
    
    static void toJSON();
    
    static void fromJSON();

    void update(GLvoid* data, size_t size, GLuint offset = 0);

    struct Widget : GUI::Window {

        int ubo_current = 0;
        int struct_current = 0;
        int elem_current = 0;
        int add_comp = 0, add_sub = 0;

        std::string add_struct, add_ubo;
         
        StringsBuffer ubo_list, struct_list, comp_list, subs_list;

        void updateUBOList() {

            std::vector<std::string> names;
            for (auto ubo:UBO::pool) { 
                // names.push_back(ubo->name); 
                names.push_back((ubo->name+" (binding:"+std::to_string(ubo->binding)+")")); 
            }
            ubo_list.create(names);
            
            if (ubo_current < UBO::pool.size()) updateStructList(); 
            
        }

        void updateStructList() {

            std::vector<std::string> names;
            for (auto &def:UBO::pool[ubo_current]->definition) { names.push_back(def.name); }
            struct_list.create(names);

        }
        
        void updateCompList() {

            std::vector<std::string> names;
            for (auto &comp:Component::pool) { names.push_back(comp->name); }
            comp_list.create(names);

        }

        void updateSubsList() {

            std::vector<std::string> names;
            for (auto &sub:UBO::pool[ubo_current]->subscribers) { names.push_back(("Shader "+std::to_string(sub->id))); }
            subs_list.create(names);

        }

        Widget() : GUI::Window("UBO Editor") { updateUBOList(); 
        add_struct.resize(60); memset(&add_struct[0],0,60); 
        add_ubo.resize(60); memset(&add_ubo[0],0,60); 
        }

        void draw() override;

    };

    static inline Widget widget;

};
#endif // UBO_H
