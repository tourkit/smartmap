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

    engine.open("project.json");

    engine.run();

}

// update all remaps in arnet update from bufferupdaet
// Buffer::instances<vec<Member,offset>>

// then tree->findSTL("x::y.z"); // dot == isData()

// Member::link ?

// remaps are owned by SRC
// remaps are listed in Remap


// remap tcp to Instance
// remap DMX to Instance
// remap Instance to Instance





//  template <typename T>
// Instance::remap_from_array(void* data) {

    // for i : src
        // dst[i] = src[i];

// }

// inputs
//    device
//       remap_nocompare (Member* src, Member* dst, char*m src_, char* dst_)
//          instance1
//          instance2




// Instance buffer::add(Member) {}

// instances have to reset on buffer change






// ~save remap

// multi layer

// linuxify

// fine

// static Effector should  update on file update

// RE QUANTITY not atlas work duh

// timelines (thus player) maybeee


// node::each<T>
