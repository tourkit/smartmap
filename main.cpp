/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "test.hpp"

int main() {

    auto &engine = Engine::getInstance();

    engine.init();

    engine.open("project2.json");

    Test test;

    engine.run();

}