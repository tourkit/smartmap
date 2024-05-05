/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"

int main() {

    engine.init();

    logger.cout();

    engine.open("project2.json");

    engine.run();

}

// ~save

// shader order

// static Effector should  update on file update
