/*

   SMARTMAP ( v 1.3.1.2 beta )

                                */

#include "member.hpp"
#include "rapidjson/document.h"
#include "src/engine.hpp"
#include "src/json.hpp"
#include <string>


int main() {

    engine.init();

    logger.cout(Sev::warning);

    engine.open("./project.json");

    engine.stack->childrens[0]->select();

    engine.run();

}
