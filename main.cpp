/*

   SMARTMAP ( v 1.3.1.2 beta )

                                */

#include "src/engine.hpp"

int main() {

    engine.init();

    engine.open("./project.json");

    engine.stack->childrens[0]->select();

    engine.run();

}
