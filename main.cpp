/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "test.hpp"

int main() {

    engine.init();

    engine.open("project2.json");

    Test test;

    engine.run();

}