/*

    SMARTMAP (beta)

                        */

#include "log.hpp"
#include "boilerplate.hpp"
#include "buffer.hpp"
#include "instance.hpp"

namespace Zob {


    struct AnyMember {

        static inline std::set<AnyMember*> pool;

    };

    template <typename T>
    struct Member : AnyMember {


    };

    struct Struct : AnyMember {

        std::vector<AnyMember*> members;

    };

    struct Buffer : Struct {


    };

    struct Instance {


    };


    struct Test {

        Test() {



        }


    };

};

int main() {

    logger.cout(true);

    {

        Struct a("x");

        Struct test1("test1");

        Struct test2("test2");

        Buffer buff;
        buff.add<float>("a").add<float>("b");

        buff[0].set<float>(1.0f);

        buff.printData();

        delete buff.members[0];

        buff.printData();

        PLOGD << "----------------";

    }
    PLOGD << "----------------2";

}

// maker update list first || do a modif(add,remove,move,resize) pool at new frame ...


// redo copy ! // gota do bkp Buffer on Node Model and Effector and DrawCall remove (thus on delete)

// finish remap
// then
// on Buffer:: add (member) { set defeualt } // do range // do default // on push
