/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "model.hpp"

int main() {

    engine.init();

    logger.cout();

    engine.open("project2.json");

    engine.run();

}

// ~save

// static Effector should  update on file update

// RE QUANTITY not atlas work duh
