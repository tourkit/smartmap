#include "struct.hpp"

Struct::Struct(std::string name, std::vector<std::string> components, int reserved)
    : Node(name), reserved(reserved) { 

    for (auto c : components) add(Component::id(c.c_str()));

}

Node* Struct::add(Node* node) { 

    auto comp = node->is_a<Component>();
    if (!comp) return nullptr;

    Node::add(new Ptr<Component>(comp));
    byte_size += comp->size;

    return node; 

}

void Struct::editor() {


 }


void Struct::push(void *data, int quantity) {

    int first_byte = buffer_offset+(this->quantity*byte_size);

    memcpy(&buffer->data[first_byte], data, quantity*byte_size);

    this->quantity += quantity;

}

char *Struct::data() { return (char*)&buffer->data[buffer_offset]; }


