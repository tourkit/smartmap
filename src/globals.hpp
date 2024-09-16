#include "member.hpp"

static struct Globals {

    Member universe;
    Member rect;
    Member layer;
    Member ubl;
    Member vertice;
    Member indice;

    Globals() :
        
        universe("Universe"),
        rect("Rect"),
        layer("Layer"),
        ubl("UBL"),
        vertice("Vertice"),
        indice("Indice")
    
    {

        universe.add<char,512>("chan").range(0,1,0);
        rect.add<float, 2>("size").add<float, 2>("pos");
        layer.quantity(0).add<float, 2>("dim").range(0, 0, 0);
        ubl.add<float, 2>("size").add<float, 2>("pos").add<float, 2>("norm").add<float, 2>("dim");
        vertice.add<float,2>("POSITION").range(-1,1,0).add<float,2>("UV").add<float,2>("NORMALIZED").add<float>("OBJ").add<float>("LAYER");
        indice.add<int, 1>("vert1", 1, 1).add<int, 1>( "vert2", 1, 1).add<int, 1>("vert3", 1, 1);

    }

} globals;