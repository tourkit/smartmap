/*

   SMARTMAP ( v 1.3.1.2 beta )

                                */

#include "engine.hpp"
#include "effector.hpp"


struct WrapperEffector : Effector {



};


int main() {


    engine.init();

    Effector::Definition wrapper;
    wrapper.s.name("wrapper");
    wrapper.args = {{"float","1"},{"float","2"},{"float","3"}};

    // engine.effectors.add

    // engine.open("project.json");

    engine.run();

}
