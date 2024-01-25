#include "node.hpp"
#include "vbo.hpp"
#include "shader.hpp"
#include <unordered_set>

struct ShaderFX;
struct DrawCall : Node {

    std::string frag_shader, vert_shader;

    VBO vbo;

    ShaderProgram shader;

    DrawCall();
    
    Node* add(Node *node);     

    void editor() override;

    void update() override;

    void run() override;
};
