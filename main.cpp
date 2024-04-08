/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "log.hpp"
#include "boilerplate.hpp"
#include "drawcall.hpp"


int main() {

    logger.cout(true);

    {


        Struct a("a");
        a.add<float>("x").add<float>("y").add<float>("z");

        Struct t("t");
        t.add(a);

        Buffer b;
        b.add(t);

        PLOGD << "----------------";

        Buffer bkp(b);

        PLOGD << bkp.print(2);
        bkp.printData();

        // PLOGD << "----------------";

        // bkp.hard_delete();

        // PLOGD << "----------------0";
        // delete bkp;
        PLOGD << "----------------1";

    }

}
