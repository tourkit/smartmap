/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "model.hpp"
#include "artnet.hpp"
#include "drawcall.hpp"

#include "ndi.hpp"

#include "buffer.hpp"
#include "struct.hpp"

int main() {

    engine.init();

    // logger.cout();

    // Struct test("test");
    // test.add<float>("x");
    // test.add<float>("y");
    // auto *test2 = new Struct("test2");
    // test2->add<float>("x");
    // test2->add<float>("y");

    // Buffer buff;
    // buff.add(test2);
    // buff.add(&test);

    // auto &i = buff[1].track();
    // PLOGD << i.offset;
    // delete test2;
    // PLOGD << i.offset;
    // PLOGD << test.quantity();
    // test.quantity(2);
    // PLOGD << i.offset;
    // test.quantity(1);
    // PLOGD << i.offset;

    engine.open("project.json");

    engine.run();

}

// big fuck on  q change

// ~save remap

// multi layer

// linuxify

// fine

// static Effector should  update on file update

// RE QUANTITY not atlas work duh

// timelines (thus player) maybeee


// node::each<T>
