#ifndef UBO_H
#define UBO_H

#include "pch.hpp"

#include "shader.hpp"
#include "component.hpp"

#include "gui.hpp"

struct UBO {

    static inline std::vector<UBO*> pool;

    static inline GLuint count = 0;

    GLuint binding = count++;

    std::vector<float> data;

    GLuint id = 0;

    std::vector<GLuint> subscribers;

    struct Struct {

        std::string name;

        std::vector<Component*> components;

        int quantity;

    };

    std::vector<Struct> definition;

    const char* name;

    ~UBO();
    
    UBO(const char* name, std::vector<GLuint> subscribers = {});

    void destroy();

    void resize();

    void addStruct(const char* name, std::vector<Component*> components, size_t quantity = 1);

    void link(GLuint shader);

    void update();
    
    static void toJSON();
    static void fromJSON();

    void update(GLvoid* data, size_t size, GLuint offset = 0);

    struct Widget : GUI::Window {

        int ubo_current = 0;
        int struct_current = 0;
        int elem_current = 0;
        int add_comp = 0;

        std::string add_struct, add_ubo;
         
        StringsBuffer ubo_list, struct_list, comp_list;

        void updateUBOList() {

            std::vector<const char*> names;
            for (auto ubo:UBO::pool) { names.push_back(ubo->name); }
            ubo_list.create(names);
            
            if (ubo_current < UBO::pool.size()) updateStructList(); 

        }

        void updateStructList() {

            std::vector<const char*> names;
            for (auto &def:UBO::pool[ubo_current]->definition) { names.push_back(def.name.c_str()); }
            struct_list.create(names);

        }
        
        void updateCompList() {

            std::vector<const char*> names;
            for (auto &comp:Component::pool) { names.push_back(comp->name.c_str()); }
            comp_list.create(names);

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
