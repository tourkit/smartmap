/*

    SMARTMAP (beta)

                        */

#include "drawcall.hpp"

#include "engine.hpp"

int main() {

    engine.init();

    logger.cout();

    engine.open("project2.json");

    auto &x = Effector::get(engine.effectors->child("argb")->is_a<File>());
    PLOGD << x.s.name();


    engine.run();

}


// default

// no instasnce at open
