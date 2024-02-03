#include "node.hpp"
#include "vbo.hpp"
#include "buffer.hpp"
#include "shader.hpp"
#include <unordered_set>

struct ShaderFX;

struct DrawCall : Node {

    Buffer buffer;

    VBO vbo;

    ShaderProgram shader;

    DrawCall();
    
    Node* add(Node *node);     

    void editor() override;

    void update() override;

    void run() override;
};
