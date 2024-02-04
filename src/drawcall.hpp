
#include "vbo.hpp"
#include "buffer.hpp"
#include "shader.hpp"
#include <unordered_set>

struct ShaderFX;

struct DrawCall {

    Buffer buffer;

    VBO *vbo;

    ShaderProgram shader;

    void update();

    void run();
};
