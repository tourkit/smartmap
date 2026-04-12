
// fais un plan, reflechis l'interet eventuel de mutualiser ou non des aspects de chaque partie , dis mois si il y a des erreurs , dans la logique, les noms des class, ...


//je veux une librarie cpp avec

//1 : un systeme de gestion (creation, modification,suppression,..) de structure dynamique 

//2 : un system de stockage avec choix AoS/SoA de ces structure 

//3 : une API et une interface web avec vue arborescence, filtre, recherche, un editeur minimaliste et complet de structure, outils de visualisation des donnée avec granularité simple et efficace 

//le tout toujours au plus single file et vanilla possible, performance et efficacité

//exemple non exhaustif des pattern de son utilisation voulue 

// is this design ok , and should Buffer be extension of Struct ?

// I would think Create() functions return some king of shared_ptr

// Field HAS SOA INFO 



#include "src/member.hpp"
#include <vector>

struct Struct;

//struct Registry {

//    static inline Registry Create() { return Registry() ; }
    
//    std::vector<Struct> strucs;

//};


struct Struct {

    static inline Struct Create(const char* label = nullptr) { return Struct() ; }

    struct Field {

        static inline Field Create() { return Field() ; }   

    };   

    std::string label_v;

    std::vector<Field> fields;

    // CRUD

    template <typename T, int Q=1, typename... Args>
    bool add(const char* label = nullptr, Args&&... args);

};

int main() {

    // es ce qu un type peut etre un struct ? looopyeah

    Registry world = Registry::Create(); // world.bind()

    Struct objet_struct = Struct::Create("Objet");

    objet_struct.add<float,2>("XY");
    objet_struct.add<int,4>("RGBA",1,1,1,1); // default values

    Struct piece_struct = Struct::Create ("Piece");

    piece_struct.add<char*>("nom"); 
    //piece_struct.add(objet_struct, "table",);
    Struct::Field chaises = piece_struct->add(objet_struct, 4, "chaises",);
    piece_struct->add<int>("ouverte",);

    Struct::Field nom_piece = objet_struct["nom"] // find field by name

    nom_objet.name("piece"); // change name to "piece"
    nom_objet.name(); // return "piece"

    objet_struct[2].type(); // return RGBA (3rd field) type "int"
    objet_struct[2].type<float>(); //  set it to float()

    objet_struct[2].destroy();
    object_struct.count() // return 2;



    Buffer buffer = Buffer::Create("Buffer"); // work like (inherit ?) struct

    buffer.type(Buffer::AoS);

    buffer.mut() // return mutable ptr to data

    auto test = buffer->add<int,3>("TEST", 1,2,3); //actual value stored

    auto testobj = buffer->add(objet_struct, "OBJ", {.5,.5},{.1,.2,.3}); // missing (here the 4th) field will be set to default

    buffer.length() // return 9 (3 for test + 6 for testobj)

    // testobj[0][1] == buffer["OBJ::XY[1]"]

        
    testobj.destroy(); // testobj == nullptr , buffer.length() return 3;

    auto test3 = buffer.add(piece_struct,2, "pieces"); // including values


    chaises.quantity(3); // then quantity() return 3

    chaises.destroy(); // chaises == nullptr;
}