/*

    SMARTMAP (beta)

                        */

#include "buffer.hpp"
#include "struct.hpp"
#include "entry.hpp"
#include "log.hpp"

int main() {

    logger.cout();

    Buffer buffy;

    auto obj = buffy.addObj(new Struct("Index",{"Vertex", "Vertex", "Vertex", "Vertex", "Vertex"}));

    auto e1 = obj->push();

    e1[0].set<uint32_t>(111);
    e1[1].set<uint32_t>(222);
    e1[2].set<uint32_t>(333);
    e1[3].set<uint32_t>(444);
    e1[4].set<uint32_t>(555);
    PLOGD<<"second";
    auto e2 = obj->push();
    
    PLOGD << e1[0][0].get<uint32_t>();
    PLOGD << e1[1][0].get<uint32_t>();
    PLOGD << e1[2][0].get<uint32_t>();
    PLOGD << e1[3][0].get<uint32_t>();
    PLOGD << e1[4][0].get<uint32_t>();


}