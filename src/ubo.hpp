#ifndef UBO_H
#define UBO_H

#include <vector>

#include "buffer.hpp"

struct ShaderProgram;
struct UBO : Node {

    GLuint id = -1;

    GLuint binding;

    static inline GLuint binding_count = 0;

    static inline Components &comps = Components::getInstance();

    std::vector<ShaderProgram*> subscribers;

    Buffer buffer;

    ~UBO();
    
    UBO(std::string name, std::vector<ShaderProgram*> subscribers = {});

    void destroy();

    void update();

    void upload();

    void upload(GLvoid* data, size_t size, GLuint offset = 0);

    Node *add(Node* node) override;

    void editor() override;
    
    // static void toJSON();
    
    // static void fromJSON();

};
#endif // UBO_H
