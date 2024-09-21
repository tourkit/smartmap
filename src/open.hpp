
#include <cstring>
#include <map>

#include "json.hpp"

struct Node;
struct Open{

    void json(std::string path);

    JSON json_v;

    void medias();
    void inputs();
    void editors();
    
    std::map<Node*,JSONVal> outputs_src;

    
};