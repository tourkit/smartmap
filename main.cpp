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

        auto bkp(b);

        bkp["t"]["a"]["z"].set<float>(1.0f);

        delete a.members[0];

        b.remap(bkp);

        PLOGD << bkp.print(2);
        bkp.printData();

        PLOGD << b.print(2);
        b.printData();


    }

}

// maker update list first || do a modif(add,remove,move,resize) pool at new frame ...


// redo copy ! // gota do bkp Buffer on Node Model and Effector and DrawCall remove (thus on delete)

// finish remap
// then
// on Buffer:: add (member) { set defeualt } // do range // do default // on push
