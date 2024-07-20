#pragma once

#include "gui.hpp"

struct Node;
struct Buffer;
struct Member;

void draw_definition(Member *member, int offset = 0, int depth = 0);

template <typename T>
struct Editor  {

    static inline std::function<void(Node*,T*)> cb = nullptr;

    Editor(std::function<void(Node*,T*)> cb) { Editor<T>::cb = cb; };

};

struct EditorWidget : GUI::Window {

    Node* selected = nullptr;

    bool locked = false;

    EditorWidget();
    ~EditorWidget();

    void draw() override;

};

struct Editors  { static void init(); };
