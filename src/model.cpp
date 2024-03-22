#include "model.hpp"

#include "effector.hpp"
#include "file.hpp"
#include "engine.hpp"
#include "struct.hpp"
#include "instance.hpp"
#include "ubo.hpp"

Model::Model(std::string name, int id, int quantity) : Struct (name+""+std::to_string(id)) { 

    this->id = id;

    this->quantity = quantity;

 }

