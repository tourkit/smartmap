/*

    SMARTMAP (beta)

                        */

#include "log.hpp"
#include "boilerplate.hpp"
#include "buffer.hpp"
#include "instance.hpp"

namespace Zob {



// write me a method for the following struct to get a list of all top member of the hierarchys "this" is part of

struct Member {

    static inline std::set<Member*> pool;

    std::vector<Member*> members;

    Member() {pool.insert(this);}

    virtual ~Member() { pool.erase(this); }

    virtual void onupdate() { /* a place for derived to do some stuff */}

    void update() {

        onupdate();

        for (auto& m : pool) if (std::find( members.begin(), members.end(), m ) != members.end()) m->update();

    }

    void add(Member* m) {

        members.push_back(m);

        update();

    }

    void remove(int i) {

        members.erase( members.begin() + i );

        update();

    }

};




    struct Test {

        Test() {



        }


    };

};


int main() {



    logger.cout(true);

    {

        // for (auto it = log().begin(); it != log().end();) { it++; }

        Struct a("x");
        a.add<float>("x").add<float>("y").add<float>("z");


        Struct test1("test1");

        // Struct test2("test2");
        // Struct test3("test3");

        test1.add(a);
        // test2.add(test1);
        // test3.add(test2);

        Buffer buff;

        buff.add(test1);

        buff.printData();
        auto bkp = buff.copy();
        // m,issing hard copy of childrens wallah
        bkp->name("buffer_bkp");
        PLOGD << bkp->print(2);
        bkp->printData();
        bkp->hard_delete();
        PLOGD << "----------------0";
        delete bkp;
        PLOGD << "----------------1";

        // auto aa = a.getTop();

        // for (auto x : aa) PLOGD << x->name();

    }
    PLOGD << "----------------2";

}

// maker update list first || do a modif(add,remove,move,resize) pool at new frame ...


// redo copy ! // gota do bkp Buffer on Node Model and Effector and DrawCall remove (thus on delete)

// finish remap
// then
// on Buffer:: add (member) { set defeualt } // do range // do default // on push
