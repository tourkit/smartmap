
#include "json.hpp"

struct Save{

    Save();

    void json(std::string path);

    JSON json_v;

    rapidjson::Document::AllocatorType& allocator;

    void medias();
    void inputs();
    void layers();
    void effectors();
    void models();
    void outputs();
    void uberlayers();
    void editors();
    
};