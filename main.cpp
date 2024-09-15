

/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */


// many vlayers

// delete vlayer should remove corresponding models

// wrapper in layer def

// feedback in layer def

// freetype

// grid / quantity 

// ----


#include "drawcall.hpp"
#include "engine.hpp"
#include "builder.hpp"
#include "instance.hpp"




// fix dynubo && statubo ..

void setDefaultToMemberInSTLChain(Member* m, std::vector<MemberQ>& stl, int offset = 0) {

    for (auto curr : stl) {
            
        for (int e = 0; e < curr.eq; e++) {

            
        }

    }

}
// how the fuck 

struct Foo{ Foo* cp = nullptr; Foo(Foo& other) : cp(&other) {} Foo() { PLOGE << "Foo"; } Foo& test() { return *this; }};
int main() {

    auto foo = Foo();
    foo.test();

    logger.cout(Sev::warning);

    auto buff = Member("buff");
    buff.buffering(true);

    auto layer = Member("layer");
    layer.quantity(2);

    buff.add(&layer);

    auto quad = Member("quad");
    quad.quantity(2); 

    layer.add(&quad);

    auto argb = Member("argb");
    argb.add<char>("ch").range(0,1,2); 

    quad.add(&argb);

    Instance inst(buff);

    inst.print();

}
