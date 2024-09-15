#pragma once

#include "gui.hpp"
#include "utils.hpp"
#include <map>
#include <functional>

struct Node;
struct Buffer;
struct Member;

void draw_definition(Member *member, int offset = 0, int depth = 0);

struct EDITOR  {

    static inline std::map<TypeIndex, void*> ptr ;

};

template <typename T>
struct Editor  {

    static inline std::function<void(EditorWidget*,Node*,T*)> cb = nullptr;

    Editor(std::function<void(EditorWidget*,Node*,T*)> cb) { 
        
        Editor<T>::cb = cb; 

        EDITOR::ptr[typeid(T)] = &Editor<T>::cb;
        
    };

};

struct EditorWidget : GUI::Window {

    Node* selected = nullptr;

    bool locked = false;

    EditorWidget(GUI* gui);
    ~EditorWidget();

    void draw() override;

};
