
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

    struct Output { int rect[4] = {1,1,0,0}; std::string name, src;};
    std::map<Node*,Output> outputs_src;

    
};