
#include "json.hpp"
#include "rapidjson/rapidjson.h"

namespace rjs  =rapidjson;

using rjs::kArrayType;
using rjs::kObjectType;

struct Save{

    Save();

    void json(std::string path);

    JSON json_v;

    rapidjson::Document::AllocatorType& allocator;

    // void medias();
    // void inputs();
    // void layers();
    // void effectors();
    // void models();
    // void outputs();
    void editors();

    void fetch(JSONVal json, Node* n);

    void saveFile(JSONVal json, Node* n);
    void saveWrappy(JSONVal json, Node* n);
    void saveWindow(JSONVal json, Node* n);
    void saveArtnet(JSONVal json, Node* n);
    void saveEffector(JSONVal json, Node* n);
    void saveModel(JSONVal json, Node* n);
    void saveLayer(JSONVal json, Node* n);
    void saveNode(JSONVal json, Node* n);
    void saveColor(rjs::Value& value, Node* n);
    void saveRemap(rjs::Value& obj, Node* n);
};