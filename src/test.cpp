#include "test.hpp"
#include "log.hpp"
#include "node.hpp"
#include "directory.hpp"
#include <cstring>
#include <typeinfo>
#include <functional>
#include <vector>

#include "engine.hpp"

namespace YOLO {


struct Buffer {

    struct Def {

        Struct s;

        int quantity, reserved;

    };

    std::vector<Def> definition;

    std::vector<char> data;  


};

struct Prout {


    Prout() {

        Buffer b;


    }
};

};


Test::Test(){


    YOLO::Prout x;




 
}