
#include <cstring>
#include <map>

#include "json.hpp"

struct Node;
struct Open{

    void json(std::string path);

    JSON json_v;


    void medias();
    void inputs();
    void layers();
    void effectors();
    void models();
    void outputs();
    void editors();

    std::map<Node*,std::string> outputs_src;

    
};